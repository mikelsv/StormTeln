# StormTeln
Graphical Telnet application. Based on my msvcore2 and ImGui.

## Reason for creation
My Windows 10 does not contained telnet. I have to write my own!

(I can't create a connection to check my server. Am I in hell already?)

## Also
My console telnet: https://github.com/mikelsv/opensource/tree/master/mteln

## Build
```
git clone https://github.com/mikelsv/StormTeln.git
git clone https://github.com/mikelsv/msvcore2.git
git clone https://github.com/ocornut/imgui.git

MSVS 2017 - 2022: StormTeln/StormTeln.vcxproj
GCC: StormTeln/Makefile
```

## Submodules
https://github.com/ocornut/imgui

https://github.com/mikelsv/msvcore2

https://github.com/BalazsJako/ImGuiColorTextEdit

## 0.0.0.4 (18.02.2025 15:25)
- [x] One window
- [x] Use Roboto fonts

## 0.0.0.3 (16.02.2025 22:31)
- [x] ImGuiColorTextEdit as terminal
- [x] Listen port
- [x] Echo options

## 0.0.0.2 (15.02.2025 14:09)
- [x] Colored output
- [x] \r\n optipons

## 0.0.0.1 (14.02.2025 11:40)
- [x] Base telnet functions. Connect to host:ip. Send data.

## t.o.d.o
- [ ] Multi Windows

- [ ] Drop files

- [ ] Scripts

- [ ] Summoning Cthulhu

## Screenshot
![StormTeln](https://github.com/mikelsv/StormTeln/blob/main/screenshot/StormTeln%200.0.0.4.jpg)
