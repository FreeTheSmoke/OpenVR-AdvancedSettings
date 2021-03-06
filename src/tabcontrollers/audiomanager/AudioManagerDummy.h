#ifndef AUDIOMANAGERLINUX_H
#define AUDIOMANAGERLINUX_H

#include "../AudioManager.h"

// application namespace
namespace advsettings
{
class AudioManagerDummy : public AudioManager
{
public:
    virtual void init( AudioTabController* controller ) override;

    virtual void setPlaybackDevice( const std::string& id,
                                    bool notify = true ) override;
    virtual std::string getPlaybackDevName() override;
    virtual std::string getPlaybackDevId() override;

    virtual void setMirrorDevice( const std::string& id,
                                  bool notify = true ) override;
    virtual bool isMirrorValid() override;
    virtual std::string getMirrorDevName() override;
    virtual std::string getMirrorDevId() override;
    virtual float getMirrorVolume() override;
    virtual bool setMirrorVolume( float value ) override;
    virtual bool getMirrorMuted() override;
    virtual bool setMirrorMuted( bool value ) override;

    virtual bool isMicValid() override;
    virtual void setMicDevice( const std::string& id,
                               bool notify = true ) override;
    virtual std::string getMicDevName() override;
    virtual std::string getMicDevId() override;
    virtual float getMicVolume() override;
    virtual bool setMicVolume( float value ) override;
    virtual bool getMicMuted() override;
    virtual bool setMicMuted( bool value ) override;

    virtual std::vector<std::pair<std::string, std::string>>
        getRecordingDevices() override;
    virtual std::vector<std::pair<std::string, std::string>>
        getPlaybackDevices() override;
};

} // namespace advsettings
#endif // AUDIOMANAGERLINUX_H
