@page audio Audio

[TOC]

This chapter discusses the audio support in EGT.

@section audio_playback Audio Playback

Playing audio in EGT is usually done with the egt::AudioPlayer class. This
class is based on GStreamer, so it supports all of the expected features for
playing various audio formats such as random seeking and pausing.

@snippet "../examples/snippets/snippets.cpp" audio1

@section sound_effect Sound Effect

For quick, asynchronous sound playback, such as in response to a button press,
the egt::SoundEffect class is more suitable than egt::AudioPlayer.

@snippet "../examples/snippets/snippets.cpp" sound1

### Supported Formats

The egt::SoundEffect class utilizes the [libsndfile library](https://libsndfile.github.io/libsndfile/) to read audio files.
The [supported formats](https://libsndfile.github.io/libsndfile/formats.html) depend on the version and compilation options of libsndfile in your distribution. For instance, MP3 support is optional and may not be available.

### Alsa Integration

The egt::SoundEffect class uses [ALSA](https://www.alsa-project.org/wiki/Main_Page) for audio playback. Due to ALSA's limited abstraction, understanding your hardware is crucial.

#### PCM Devices

Sound cards vary in their features, such as mixing capabilities, supported
formats, channels and sample rates. EGT modifies the ALSA configuration to use
ALSA plugins, creating EGT PCM devices with consistent features. The hardware
PCM devices are wrapped in the dmix plugin for mixing capabilities and the plug
plugin for channel, rate, and format conversion. This configuration only affects
the EGT application process.

Upon the first instantiation of an egt::SoundEffect object, EGT detects all ALSA
sound cards and modifies the ALSA configuration to create EGT PCM devices based
on the hardware PCM devices.

PCM devices are identified by a card index and a device index, which can be
retrieved using the `aplay` command:
```
root@sam9x75-curiosity-sd:~# aplay -l
**** List of PLAYBACK Hardware Devices ****
card 0: CLASSD [CLASSD], device 0: CLASSD PCM snd-soc-dummy-dai-0 [CLASSD PCM snd-soc-dummy-dai-0]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
card 1: sam9x60curiosit [wm8731 @ sam9x60-curiosity], device 0: WM8731 HiFi wm8731-hifi-0 [WM8731 HiFi wm8731-hifi-0]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
card 2: WH3022 [Dell Headset WH3022], device 0: USB Audio [USB Audio]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
```
For example, to use the CLASSD sound card, the card index is 0 and the device
index is 0.

You can also use `alsactl info` command for more information.

#### Volume Control

EGT doesn't manage volume control due to the variability in sound card
configurations. Channel names and the need to unmute some channels can differ
between cards. Use `alsamixer` to configure playback volume and `alsa ctl store`
to save the settings.

#### Playback

Audio file playback occurs in a separate thread. The `play()` method starts playback, and the `stop()` method interrupts it. If repeat mode is enabled using the `repeat()` method, the sound file will continue to play until repeat mode is disabled or playback is stopped.
