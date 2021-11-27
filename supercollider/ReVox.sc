/*_____________________________________________________________
Control Revox Machines Remotely from SC

(C) 2020 Jonathan Reus / GPLv3
http://jonathanreus.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

________________________________________________________________*/


/*_____________________________________________________________
USAGE:
Rev.connect();
Rev.verbose = false;

Rev.cmd(0, \stop, 100);
Rev.cmd(1, \stop, 100);
Rev.cmd2(0, \rew, 100);
Rev.cmd2(1, \rew, 100);
Rev.cmd2(0, \play, 100);
Rev.cmd2(1, \play, 100);
Rev.stop(0);
Rev.stop(1);


Rev.close();

________________________________________________________________*/



Rev {
  classvar <baud = 115200; // serial baud rate
  classvar <>verbose = true; // output serial status messages
  classvar <serialPort = nil;
  classvar <serialDevice = nil;
  classvar <>serialDevicePattern = "/dev/tty.usb*"; // Arduino on mac
  classvar <serialRoutine;
  classvar <>numMachines = 2;
  classvar <runAsDummy=false;

  // magic numbers...
  classvar <>maxSpeed=130, <>minSpeed=240; // mapping range to Arduino PWM byte values for capstan control

  // Mapping of commands to opcodes
  classvar <cmdMap;

  *initClass {
    // Mapping of commands to serial op-codes
    cmdMap = (stop: 3, play: 9, rec: 19, pause: 22, rew: 33, fwd: 41, capstan: 27, spd: 43);
  }

  // runAsDummy flag indicates whether the class should be
  // responsive even without an active serial connection, to be
  // used while testing...
  *connect {|testMode=false|
    if(serialPort.notNil) {
      serialPort.close; serialPort = nil;
    };

    runAsDummy=testMode;
    if(runAsDummy.not) {
    SerialPort.devicePattern = serialDevicePattern;
    "Matching serial devices found: %".format(SerialPort.devices).warn;
    serialDevice = (SerialPort.devices)[0];
    if(serialDevice.isNil) {
      Error("No device found on serial port! Looking for device pattern %".format(serialDevicePattern)).throw;
    };
    serialPort = SerialPort(serialDevice, baudrate: baud, crtscts: true);
    serialPort.doneAction = { warn("!!!, SOMETHING CLOSED THE SERIAL PORT !!!"); };

    if(serialRoutine.notNil) {
      serialRoutine.stop; serialRoutine = nil;
    };

    serialRoutine = {
      if(serialPort.isOpen) {
        inf.do {|idx|
          var byte;
          byte = serialPort.read;
          if(byte.notNil) {
            if(verbose) {
              byte.asAscii.post;
            };
          };
          0.001.wait;
        }
      }
    }.fork(SystemClock);

    CmdPeriod.add({
      serialRoutine.stop;
      serialPort.close;
    });
    };
    "Revox: Serial port connected.".warn;
  }

  // send a command to a specific machine id
  *cmd {|machine_id=0, command=\stop, extra=0, redundancy=1|
    var data_code, command_code;
    if(runAsDummy.not && serialPort.isNil) {
      Error("Revox: Serial port not initialized when attempting command % on machine %".format(command, machine_id)).throw;
    };
    if(machine_id.inclusivelyBetween(0,numMachines).not) {
      Error("Invalid machine ID '%'".format(machine_id)).throw;
    };
    data_code = ($A.asInt + machine_id).asAscii;
    command_code = cmdMap[command];
    if(command_code.isNil) {
      Error("Invalid machine command '%'".format(command)).throw;
    };
    if(runAsDummy.not) {
    redundancy.do {
      serialPort.putAll(Int8Array[244, 253,
        data_code.asInt, command_code, extra >> 8, extra & 0xff
      ]);
    };
    } {
      "Revox %: % % xx %".format(machine_id, command, extra, redundancy).warn;
    };
  }

  // convenience for triple redundant command send
  *cmd3 {|machine_id=0, command=\stop, extra=0|
    this.cmd(machine_id, command, extra, 3);
  }
  // more convenience methods
  *stop {|machine_id=0, redundancy=3|
    this.cmd(machine_id, \stop, 100, redundancy);
  }
  // dur in seconds
  *pause {|machine_id=0, dur=1.0, redundancy=2|
    var dur_ms = (dur * 1000).asInteger;
    this.cmd(machine_id, \pause, dur_ms, redundancy);
  }
  *play {|machine_id=0, redundancy=3|
    this.cmd(machine_id, \play, 100, redundancy);
  }
  *rec {|machine_id=0, redundancy=3|
    this.cmd(machine_id, \rec, 100, redundancy);
  }
  *rew {|machine_id=0, redundancy=3|
    this.cmd(machine_id, \rew, 100, redundancy);
  }
  *fwd {|machine_id=0, redundancy=3|
    this.cmd(machine_id, \fwd, 100, redundancy);
  }
  // status=0 off, status=1 on
  *capstan {|machine_id=0, status=0, redundancy=2|
    if(status != 0) { status = 1 };
    this.cmd(machine_id, \capstan, status, redundancy);
  }
  // playback speed 0.0 - 1.0 (mapped to control voltages)
  *spd {|machine_id=0, speed=0.5, redundancy=2|
    var pwm_voltage = speed.clip(0.0, 1.0).linlin(0.0, 1.0, minSpeed, maxSpeed).asInteger;
    this.cmd(machine_id, \spd, pwm_voltage, redundancy);
  }

  // Stop everything
  *close {
    numMachines.do {|id|
      stop(id);
    };
    serialRoutine.stop;
    serialPort.close;
  }
}