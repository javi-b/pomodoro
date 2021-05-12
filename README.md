# pomodoro
Simple Pomodoro Timer for GNU+Linux using sound and text notifications. 

## Installation
```
user $ git clone https://github.com/javi-b/pomodoro.git
user $ cd pomodoro
user $ make
root # make install 
```

## Usage
Run the command:
```
user $ pomodoro
```
It will send sound and text notifications at the start of any work or break periods from the [Pomodoro Technique](https://en.wikipedia.org/wiki/Pomodoro_Technique). The default periods last:
Work | Short break | Long break
---- | ----------- | ----------
25 min. | 5 min. | 30 min.

## Configuration
You can change the periods duration and the notifications sound and messages by changing constants in the source code. After that, recompile and reinstall the programm:
```
user $ make
root # make install
```

## Dependencies
* `notify-send`
* `mpv`
