#include "MoveCenterTabController.h"
#include <QQuickWindow>
#include "../overlaycontroller.h"

void rotateCoordinates( float coordinates[3], float angle )
{
    if ( angle == 0 )
    {
        return;
    }
    float s = sin( angle );
    float c = cos( angle );
    float newX = coordinates[0] * c - coordinates[2] * s;
    float newZ = coordinates[0] * s + coordinates[2] * c;
    coordinates[0] = newX;
    coordinates[2] = newZ;
}

// application namespace
namespace advsettings
{
using std::chrono::duration_cast;
using std::chrono::milliseconds;
typedef std::chrono::system_clock clock;

void MoveCenterTabController::initStage1()
{
    setTrackingUniverse( vr::VRCompositor()->GetTrackingSpace() );
    auto settings = OverlayController::appSettings();
    settings->beginGroup( "playspaceSettings" );
    auto value = settings->value( "adjustChaperone", m_adjustChaperone );
    if ( value.isValid() && !value.isNull() )
    {
        m_adjustChaperone = value.toBool();
    }
    value = settings->value( "moveShortcutRight", m_moveShortcutRightEnabled );
    if ( value.isValid() && !value.isNull() )
    {
        m_moveShortcutRightEnabled = value.toBool();
    }
    value = settings->value( "moveShortcutLeft", m_moveShortcutLeftEnabled );
    if ( value.isValid() && !value.isNull() )
    {
        m_moveShortcutLeftEnabled = value.toBool();
    }
    value = settings->value( "requireDoubleClick", m_requireDoubleClick );
    if ( value.isValid() && !value.isNull() )
    {
        m_requireDoubleClick = value.toBool();
    }
    value = settings->value( "lockXToggle", m_lockXToggle );
    if ( value.isValid() && !value.isNull() )
    {
        m_lockXToggle = value.toBool();
    }
    value = settings->value( "lockYToggle", m_lockYToggle );
    if ( value.isValid() && !value.isNull() )
    {
        m_lockYToggle = value.toBool();
    }
    value = settings->value( "lockZToggle", m_lockZToggle );
    if ( value.isValid() && !value.isNull() )
    {
        m_lockZToggle = value.toBool();
    }
    settings->endGroup();
    lastMoveButtonClick[0] = lastMoveButtonClick[1] = clock::now();
}

void MoveCenterTabController::initStage2( OverlayController* var_parent,
                                          QQuickWindow* var_widget )
{
    this->parent = var_parent;
    this->widget = var_widget;
}

int MoveCenterTabController::trackingUniverse() const
{
    return ( int ) m_trackingUniverse;
}

void MoveCenterTabController::setTrackingUniverse( int value, bool notify )
{
    if ( m_trackingUniverse != value )
    {
        reset();
        m_trackingUniverse = value;
        if ( notify )
        {
            emit trackingUniverseChanged( m_trackingUniverse );
        }
    }
}

float MoveCenterTabController::offsetX() const
{
    return m_offsetX;
}

void MoveCenterTabController::setOffsetX( float value, bool notify )
{
    if ( m_offsetX != value )
    {
        modOffsetX( value - m_offsetX, notify );
    }
}

float MoveCenterTabController::offsetY() const
{
    return m_offsetY;
}

void MoveCenterTabController::setOffsetY( float value, bool notify )
{
    if ( m_offsetY != value )
    {
        modOffsetY( value - m_offsetY, notify );
    }
}

float MoveCenterTabController::offsetZ() const
{
    return m_offsetZ;
}

void MoveCenterTabController::setOffsetZ( float value, bool notify )
{
    if ( m_offsetZ != value )
    {
        modOffsetZ( value - m_offsetZ, notify );
    }
}

int MoveCenterTabController::rotation() const
{
    return m_rotation;
}

void MoveCenterTabController::setRotation( int value, bool notify )
{
    if ( m_rotation != value )
    {
        // float angle = (value - m_rotation) * 2 * M_PI / 360.0;
        // parent->RotateUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse,
        // angle, m_adjustChaperone);
        m_rotation = value;
        if ( notify )
        {
            emit rotationChanged( m_rotation );
        }
    }
}

void MoveCenterTabController::applyRotation()
{
    if ( m_rotation != m_rotationOld )
    {
        float angle = ( m_rotationOld - m_rotation ) * 2 * M_PI / 360.0;
        parent->RotateUniverseCenter(
            ( vr::TrackingUniverseOrigin ) m_trackingUniverse,
            angle,
            m_adjustChaperone );
        m_rotationOld = m_rotation;
    }
}

bool MoveCenterTabController::adjustChaperone() const
{
    return m_adjustChaperone;
}

void MoveCenterTabController::setAdjustChaperone( bool value, bool notify )
{
    if ( m_adjustChaperone != value )
    {
        m_adjustChaperone = value;
        if ( m_trackingUniverse == vr::TrackingUniverseStanding )
        {
            auto angle = m_rotation * 2 * M_PI / 360.0;
            float offsetdir = m_adjustChaperone ? -1.0 : 1.0;
            float offset[3] = { offsetdir * m_offsetX,
                                offsetdir * m_offsetY,
                                offsetdir * m_offsetZ };
            rotateCoordinates( offset, angle );

            parent->AddOffsetToCollisionBounds( offset );
        }
        auto settings = OverlayController::appSettings();
        settings->beginGroup( "playspaceSettings" );
        settings->setValue( "adjustChaperone", m_adjustChaperone );
        settings->endGroup();
        settings->sync();
        if ( notify )
        {
            emit adjustChaperoneChanged( m_adjustChaperone );
        }
    }
}

bool MoveCenterTabController::moveShortcutRight() const
{
    return m_moveShortcutRightEnabled;
}

void MoveCenterTabController::setMoveShortcutRight( bool value, bool notify )
{
    m_moveShortcutRightEnabled = value;
    auto settings = OverlayController::appSettings();
    settings->beginGroup( "playspaceSettings" );
    settings->setValue( "moveShortcutRight", m_moveShortcutRightEnabled );
    settings->endGroup();
    settings->sync();
    if ( notify )
    {
        emit moveShortcutRightChanged( m_moveShortcutRightEnabled );
    }
}

bool MoveCenterTabController::moveShortcutLeft() const
{
    return m_moveShortcutLeftEnabled;
}

void MoveCenterTabController::setMoveShortcutLeft( bool value, bool notify )
{
    m_moveShortcutLeftEnabled = value;
    auto settings = OverlayController::appSettings();
    settings->beginGroup( "playspaceSettings" );
    settings->setValue( "moveShortcutLeft", m_moveShortcutLeftEnabled );
    settings->endGroup();
    settings->sync();
    if ( notify )
    {
        emit moveShortcutLeftChanged( m_moveShortcutLeftEnabled );
    }
}

bool MoveCenterTabController::requireDoubleClick() const
{
    return m_requireDoubleClick;
}

void MoveCenterTabController::setRequireDoubleClick( bool value, bool notify )
{
    m_requireDoubleClick = value;
    auto settings = OverlayController::appSettings();
    settings->beginGroup( "playspaceSettings" );
    settings->setValue( "requireDoubleClick", m_requireDoubleClick );
    settings->endGroup();
    settings->sync();
    if ( notify )
    {
        emit requireDoubleClickChanged( m_requireDoubleClick );
    }
}

bool MoveCenterTabController::lockXToggle() const
{
    return m_lockXToggle;
}

void MoveCenterTabController::setLockX( bool value, bool notify )
{
    m_lockXToggle = value;
    auto settings = OverlayController::appSettings();
    settings->beginGroup( "playspaceSettings" );
    settings->setValue( "lockXToggle", m_lockXToggle );
    settings->endGroup();
    settings->sync();
    if ( notify )
    {
        emit requireLockXChanged( m_lockXToggle );
    }
}

bool MoveCenterTabController::lockYToggle() const
{
    return m_lockYToggle;
}

void MoveCenterTabController::setLockY( bool value, bool notify )
{
    m_lockYToggle = value;
    auto settings = OverlayController::appSettings();
    settings->beginGroup( "playspaceSettings" );
    settings->setValue( "lockYToggle", m_lockYToggle );
    settings->endGroup();
    settings->sync();
    if ( notify )
    {
        emit requireLockYChanged( m_lockYToggle );
    }
}

bool MoveCenterTabController::lockZToggle() const
{
    return m_lockZToggle;
}

void MoveCenterTabController::setLockZ( bool value, bool notify )
{
    m_lockZToggle = value;
    auto settings = OverlayController::appSettings();
    settings->beginGroup( "playspaceSettings" );
    settings->setValue( "lockZToggle", m_lockZToggle );
    settings->endGroup();
    settings->sync();
    if ( notify )
    {
        emit requireLockZChanged( m_lockZToggle );
    }
}

void MoveCenterTabController::modOffsetX( float value, bool notify )
{
    // TODO ? possible issue with locking position this way
    if ( !m_lockXToggle )
    {
        auto angle = m_rotation * 2 * M_PI / 360.0;
        float offset[3] = { value, 0, 0 };
        rotateCoordinates( offset, angle );
        parent->AddOffsetToUniverseCenter(
            ( vr::TrackingUniverseOrigin ) m_trackingUniverse,
            offset,
            m_adjustChaperone,
            false );
        m_offsetX += value;
        if ( notify )
        {
            emit offsetXChanged( m_offsetX );
        }
    }
}

void MoveCenterTabController::modOffsetY( float value, bool notify )
{
    if ( !m_lockYToggle )
    {
        parent->AddOffsetToUniverseCenter(
            ( vr::TrackingUniverseOrigin ) m_trackingUniverse,
            1,
            value,
            m_adjustChaperone,
            false );
        m_offsetY += value;
        if ( notify )
        {
            emit offsetYChanged( m_offsetY );
        }
    }
}

void MoveCenterTabController::modOffsetZ( float value, bool notify )
{
    if ( !m_lockZToggle )
    {
        auto angle = m_rotation * 2 * M_PI / 360.0;
        float offset[3] = { 0, 0, value };
        rotateCoordinates( offset, angle );
        parent->AddOffsetToUniverseCenter(
            ( vr::TrackingUniverseOrigin ) m_trackingUniverse,
            offset,
            m_adjustChaperone,
            false );
        m_offsetZ += value;
        if ( notify )
        {
            emit offsetZChanged( m_offsetZ );
        }
    }
}

void MoveCenterTabController::reset()
{
    vr::VRChaperoneSetup()->RevertWorkingCopy();
    parent->RotateUniverseCenter(
        ( vr::TrackingUniverseOrigin ) m_trackingUniverse,
        -m_rotation * 2 * M_PI / 360.0,
        m_adjustChaperone,
        false );
    float offset[3] = { -m_offsetX, -m_offsetY, -m_offsetZ };
    parent->AddOffsetToUniverseCenter(
        ( vr::TrackingUniverseOrigin ) m_trackingUniverse,
        offset,
        m_adjustChaperone,
        false );
    m_offsetX = 0.0f;
    m_offsetY = 0.0f;
    m_offsetZ = 0.0f;
    m_rotation = 0;
    emit offsetXChanged( m_offsetX );
    emit offsetYChanged( m_offsetY );
    emit offsetZChanged( m_offsetZ );
    emit rotationChanged( m_rotation );
}

bool isMoveShortCutPressed( vr::ETrackedControllerRole hand )
{
    auto handId
        = vr::VRSystem()->GetTrackedDeviceIndexForControllerRole( hand );
    if ( handId == vr::k_unTrackedDeviceIndexInvalid
         || handId >= vr::k_unMaxTrackedDeviceCount )
    {
        return false;
    }

    vr::VRControllerState_t state;
    if ( vr::VRSystem()->GetControllerState(
             handId, &state, sizeof( vr::VRControllerState_t ) ) )
    {
        return state.ulButtonPressed
               & vr::ButtonMaskFromId( vr::k_EButton_ApplicationMenu );
    }

    return false;
}

vr::ETrackedControllerRole MoveCenterTabController::getMoveShortcutHand()
{
    auto activeHand = m_activeMoveController;

    bool rightPressed
        = m_moveShortcutRightEnabled
          && isMoveShortCutPressed( vr::TrackedControllerRole_RightHand );
    bool leftPressed
        = m_moveShortcutLeftEnabled
          && isMoveShortCutPressed( vr::TrackedControllerRole_LeftHand );
    bool checkDoubleClick = m_requireDoubleClick
                            && activeHand == vr::TrackedControllerRole_Invalid;

    auto now = clock::now();
    // if we start pressing the shortcut on a controller we set the active one
    // to it
    if ( rightPressed && !m_moveShortcutRightPressed )
    {
        auto elapsed
            = duration_cast<milliseconds>( now - lastMoveButtonClick[0] )
                  .count();
        if ( !checkDoubleClick || elapsed < 250 )
        {
            activeHand = vr::TrackedControllerRole_RightHand;
        }
        lastMoveButtonClick[0] = now;
    }
    if ( leftPressed && !m_moveShortcutLeftPressed )
    {
        auto elapsed
            = duration_cast<milliseconds>( now - lastMoveButtonClick[1] )
                  .count();
        if ( !checkDoubleClick || elapsed < 250 )
        {
            activeHand = vr::TrackedControllerRole_LeftHand;
        }
        lastMoveButtonClick[1] = now;
    }

    // if we let down of a shortcut we set the active hand to any remaining
    // pressed down hand
    if ( !rightPressed && m_moveShortcutRightPressed )
    {
        activeHand = leftPressed ? vr::TrackedControllerRole_LeftHand
                                 : vr::TrackedControllerRole_Invalid;
    }
    if ( !leftPressed && m_moveShortcutLeftPressed )
    {
        activeHand = rightPressed ? vr::TrackedControllerRole_RightHand
                                  : vr::TrackedControllerRole_Invalid;
    }

    if ( !leftPressed && !rightPressed )
    {
        activeHand = vr::TrackedControllerRole_Invalid;
        if ( m_activeMoveController != vr::TrackedControllerRole_Invalid )
        {
            // grace period while switching hands
            lastMoveButtonClick[0] = lastMoveButtonClick[1] = now;
        }
    }

    m_activeMoveController = activeHand;
    m_moveShortcutRightPressed = rightPressed;
    m_moveShortcutLeftPressed = leftPressed;

    return activeHand;
}

void MoveCenterTabController::eventLoopTick(
    vr::ETrackingUniverseOrigin universe,
    vr::TrackedDevicePose_t* devicePoses )
{
    if ( settingsUpdateCounter >= 50 )
    {
        setTrackingUniverse( ( int ) universe );
        settingsUpdateCounter = 0;
    }
    else
    {
        settingsUpdateCounter++;
        auto oldMoveHand = m_activeMoveController;
        auto newMoveHand = getMoveShortcutHand();
        if ( newMoveHand == vr::TrackedControllerRole_Invalid )
        {
            emit offsetXChanged( m_offsetX );
            emit offsetYChanged( m_offsetY );
            emit offsetZChanged( m_offsetZ );
            return;
        }
        auto handId = vr::VRSystem()->GetTrackedDeviceIndexForControllerRole(
            newMoveHand );
        if ( newMoveHand == vr::TrackedControllerRole_Invalid
             || handId == vr::k_unTrackedDeviceIndexInvalid
             || handId >= vr::k_unMaxTrackedDeviceCount )
        {
            if ( oldMoveHand != vr::TrackedControllerRole_Invalid )
            {
                emit offsetXChanged( m_offsetX );
                emit offsetYChanged( m_offsetY );
                emit offsetZChanged( m_offsetZ );
            }
            return;
        }
        vr::TrackedDevicePose_t* pose = devicePoses + handId;
        if ( !pose->bPoseIsValid || !pose->bDeviceIsConnected
             || pose->eTrackingResult != vr::TrackingResult_Running_OK )
        {
            return;
        }

        float relativeControllerPosition[]
            = { pose->mDeviceToAbsoluteTracking.m[0][3],
                pose->mDeviceToAbsoluteTracking.m[1][3],
                pose->mDeviceToAbsoluteTracking.m[2][3] };

        auto angle = m_rotation * 2 * M_PI / 360.0;
        rotateCoordinates( relativeControllerPosition, -angle );
        float absoluteControllerPosition[] = {
            relativeControllerPosition[0] + m_offsetX,
            relativeControllerPosition[1] + m_offsetY,
            relativeControllerPosition[2] + m_offsetZ,
        };

        if ( oldMoveHand == newMoveHand )
        {
            float diff[3] = {
                absoluteControllerPosition[0] - m_lastControllerPosition[0],
                absoluteControllerPosition[1] - m_lastControllerPosition[1],
                absoluteControllerPosition[2] - m_lastControllerPosition[2],
            };

            // offset is un-rotated coordinates

            // prevents UI from updating if axis movement is locked
            if ( !m_lockXToggle )
            {
                m_offsetX += diff[0];
            }
            if ( !m_lockYToggle )
            {
                m_offsetY += diff[1];
            }
            if ( !m_lockZToggle )
            {
                m_offsetZ += diff[2];
            }

            rotateCoordinates( diff, angle );
            // If locked removes movement
            if ( m_lockXToggle )
            {
                diff[0] = 0;
            }
            if ( m_lockYToggle )
            {
                diff[1] = 0;
            }
            if ( m_lockZToggle )
            {
                diff[2] = 0;
            }

            parent->AddOffsetToUniverseCenter(
                ( vr::TrackingUniverseOrigin ) m_trackingUniverse,
                diff,
                m_adjustChaperone,
                false );
        }
        m_lastControllerPosition[0] = absoluteControllerPosition[0];
        m_lastControllerPosition[1] = absoluteControllerPosition[1];
        m_lastControllerPosition[2] = absoluteControllerPosition[2];
    }
}

} // namespace advsettings
