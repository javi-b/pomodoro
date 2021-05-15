# pomodoro
Simple Pomodoro Timer for GNU+Linux using sound and text notifications. 

## Install
```
user $ git clone https://github.com/javi-b/pomodoro.git
user $ cd pomodoro
user $ make
root # make install 
```

## Uninstall
```
root # make uninstall
user $ rm -rf pomodoro
```

## Usage
Run the command `pomodoro` in the terminal. It will send sound and text notifications at the start of any work or break periods from the [Pomodoro Technique](https://en.wikipedia.org/wiki/Pomodoro_Technique). The default periods last:
Work | Short break | Long break
---- | ----------- | ----------
25 min | 5 min | 30 min

However, you can change the default values and other options using command flags. See all the options running `pomodoro --help`.

If you want to run the process in the backgroud just run `pomodoro & disown` to start it and `pkill pomodoro` to stop it.

### Examples
Run pomodoro in the background without sound and changing the long break duration to 20 minutes:
```
user $ pomodoro --no-sound --long=20 & diswon
```
Run pomodoro with notifications in the terminal instead of in the desktop:
```
user $ pomodoro --verbose --no-desktop
```

## Dependencies
* `notify-send`
* `mpv`
