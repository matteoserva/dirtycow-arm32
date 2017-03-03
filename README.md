# dirtycow-arm32


This short guide will explain how I got unrestricted root access on my phone.
It should work for any arm32 android with dirtyc0w support

required software:
* arm compiler toolchain, I suggest the android-ndk-compiler
* android libsepol: https://android.googlesource.com/platform/external/libsepol
* adb, android debug bridge

suggested software:
* ida pro. Demo version is fine

required skills:
* compiling software for android
* basic assembly knowledge

suggested skills:
* writing arm shellcode

thanks:
* https://www.redtile.io/security/galaxy/
* https://github.com/freddierice/trident
* https://github.com/timwr/CVE-2016-5195
* https://github.com/phhusson/sepolicy-inject


STEPS:
* Obtain read access to /init
* Patch /init
* Replace policy


# Obtain read access to /init

You usually won't have read permission on the /init file, thus you need to exploit a process that can.
My pivot point was the dnsmasq process which can be started by activating the wifi tethering.
wpa_supplicant and mount are equally valid processes.

The folder /dirtycow in this repository contains the source code I used.
dirtycow.c is the dirtycow exploit, sh.c spawns a shell on port 11111 with the same privilege of the running process.

If your pivot process doesn't have the privilege to launch sh or create a tcp server, then you'll have to use a different code.

Once you have access to init, copy it on your sdcard, then use adb pull to recover it on your pc

# Patch /init

 This is the difficult part, you need to find the function bootchart_sample() to patch it.
 I wrote a shellcode that loads a new policy from /data/local/tmp/sepolicy and loads it into the system

 Open /init in ida pro and compare it with the source code.
 For android marshmallow the source is on google repository:
 https://android.googlesource.com/platform/system/core/+/android-6.0.1_r78/init/init.cpp
 My /init used branches to exit from some functions and IDA couldn't analyze them, I solved that by manually setting the end of the function( Edit/Functions/set function end)

 The bootchart_sample() should look like the one in the /init_ida directory.

 Now it's time to compile the shellcode.

 The shellcode is in the /shellcode directory

 To compile I used gcc: arm-linux-androideabi-as shellcode.s

 The relevant part is between AAAAINIT and AAAAFINE. Ignore the forkexec code, I used it for debugging to execute my shellcode in a child process without killing the orinal init.

 Open the compiled shellcode in a hex editor and copy the relevant part, use the strings AAAAINIT and AAAAFINE to locate it.

 Open your init file and copy the shellcode just after the prologue of the bootchart_sample() function.

The result should look like the image in the /init_ida directory.

Since we don't want to crash /init, you also have to manually add the epilogue.

My bootchart_sample() process started with a "PUSH.W          {R4-R11,LR}" so I added a "POP.W           {R4-R11,PC}; NOP" at the end of the shellcode.

You can use converter at http://armconverter.com/ to create the code or use gcc again.

Look at the file init_patched.txt to see the full patched function and its epilogue.

Now you have to load the new init file. To speed up things, you have to make init sleep so it isn't executing bootchart_sample while we are overwriting it.
The best way is to turn off the screen and wait 10 seconds. Then dirtyc0w it.

An alternative way is to follow the 3 steps procedure at redtile.io

# Replace policy

Download the android version of libsepol and compile it.
Download the sepolicy-inject and compile it.

Notice that sepolicy-inject has more functions that the ones listed in the README (look at the --auto and --not parameters)
Download your /sepolicy
Patch it to add your permissions
Upload it to /data/local/tmp/sepolicy

Now launch "adb logcat" log for audit message. You should see a message indicating that your policy is loaded

# Create the privileged context

(to be completed)
go in /sepolicy_generator and modify your policy

# The su daemon

(to be completed )
	
The SuperDaemon must be launched as root. I replaced dnsmasq with it so I can start the daemon by activating wifi tethering.

Once the daemon is activated, you can run ./su (the same file) to connect to the daemon and get root permission.

Create a folder as system_data_file, readable by everyone, and put it /data/local/tmp2 so everyone can use it.

# Make the new policy permanent

( to be completed )
* save the new policy in /data/security or /data/security/current to make it autoload at boot
* the new policy should allow writing /sys/kernel/uevent_helper
* the new policy should allow launching adb client
* create cap_setuid binary in /data/enc_user that writes uevent_helper
* from user, setprop service.adb.tcp.port, connect to self, launch setuid binary, write uevent_helper
* from uevent_helper spawn su daemon


Make it survive a OS update

# alternative dirtycow targets.

* you can dirtycow the default /sepolicy and trigger a reload
* you can dirtycow the default libc or any shared library and wait for a process to call your code
* you can live patch netd to fork and exec to your shell
* you can dirtycow a suid file and run it from gdb which has setuid capability
