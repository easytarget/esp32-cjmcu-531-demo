// Lidar Base
// A bit overdone, but works OK. 
// All the dimensions of the Boards are resizable
//  and boards can be moved relative to each other 
//  for clearance.

$fn=90;

// Fitting mode is good for getting the design right
fitting=!false;

// Parts & Dimensions

// ESP32 Dev Board 
// (this can vary, there are many 'standard' dev board layouts for the ESP32)
esp32BoardX=28.7;  // overall board dimensions
esp32BoardY=51.7;
esp32Holex=23.5;   // holes (assume common centre to board)
esp32HoleY=47;
esp32HoleD=3;      // hole diameter

// Stepper Motor H-Bridge driver board
hBridgeBoardX=34.5;
hBridgeBoardY=32;
hBridgeHoleX=29.5;
hBridgeHoleY=26.5;
hBridgeHoleD=3;

// Stepper Motor
stepperMotorDiameter=28;
stepperMotorHeight=19.25;
stepperMotorWireout=17.5;
stepperMotorHoleW=35;
stepperMotorHoleD=4.2;
stepperMotorShaft=5;
stepperMotorShaftFlat=3;
stepperMotorShaftOffset=7.5;
stepperMotorShaftBossDiameter=9.3;
stepperMotorShaftLength=8.5; 

// VL53L1X module
sensorBoardH=12.5;
sensorBoardW=15.8;
sensorHoleW=20;
sensorHoleD=3.5;

// Offsets for boards along Y axis (length)
esp32Y = -40;
hBridgeY = 5;
stepperMotorY = 40;

// Lay the parts out
if (!fitting)  {
    stand();
    mount([0,50,26],-90); 
} else {
    stand();
    mount(pos=[0,stepperMotorY+stepperMotorShaftOffset-3.5,50],rot=-90);
    color("darkgreen",0.8) esp32(pos=[0,esp32Y,20]);
    color("green",0.8) hBridge(pos=[0,hBridgeY,12]);
    color("grey",0.8) stepperMotor(pos=[0,stepperMotorY,0]);
    color("midnightblue",0.8) sensor(pos=[0,stepperMotorY+stepperMotorShaftOffset+4.5,50.7],rot=95);
}

// The VL53L1X sensor mount (yoke)
module mount(pos=[0,0,0],rot=0) translate(pos) rotate([rot,0,0]) {
    translate([0,0,-1]) rotate([5,0,0])
    linear_extrude(height=7.5) {
        translate([-sensorHoleW/2,0])
        difference() {
            circle(d=sensorHoleD*1.5);
            rotate(30) circle(d=sensorHoleD*0.75,$fn=6);
        }
        translate([sensorHoleW/2,0])
        difference() {
            circle(d=sensorHoleD*1.5);
            rotate(30) circle(d=sensorHoleD*0.75,$fn=6);
        }
    }
    difference() {
        hull () {
            translate([10,23,0]) scale([0.5,1,1])
            cylinder(d=6,h=7);
            translate([-10,23,0]) scale([0.5,1,1])
            cylinder(d=6,h=7);
        }
        translate([0,23,3.5])
        rotate([90,0,0])
        intersection() {
            cylinder(d=stepperMotorShaft+0.3,h=7,center=true);
            cube([stepperMotorShaft+1,stepperMotorShaftFlat,7],center=true);
        }
        translate([0,23,3.5])
        cube([15,7,0.6],center=true);
    }
    hull () {
        translate([10,23,0]) scale([0.5,1,1])
        cylinder(d=6,h=7);
        translate([10,2,0])
        rotate([5,0,0])
        scale([1,0.1,1]) 
        cylinder(d=3,h=6);
    }
    hull () {
        translate([-10,23,0]) scale([0.5,1,1])
        cylinder(d=6,h=7);
        translate([-10,2,0])
        rotate([5,0,0])
        scale([1,0.1,1])
        cylinder(d=3,h=6);
    }
}

// The main base/stand for the unit
module stand() {
    // motor mounts
    translate([0,stepperMotorY,0])
    linear_extrude(height=stepperMotorHeight-0.6) {
        for(x=[-0.5,0.5]) {
            translate([stepperMotorHoleW*x,0])
            difference() {
                scale([0.7,1]) circle(r=stepperMotorHoleD);
                rotate(30) circle(d=stepperMotorHoleD*0.7,$fn=6);
            }
        }
    }

    // driver mounts
    translate([0,hBridgeY,0])
    linear_extrude(height=12) {
        for(x=[-0.5,0.5],y=[-0.5,0.5]) {
            translate([hBridgeHoleX*x,hBridgeHoleY*y])
            difference() {
                circle(r=hBridgeHoleD);
                circle(d=hBridgeHoleD*0.75,$fn=6);
            }
        }
    }
        
    // esp32 mounts
    translate([0,esp32Y,0]) 
    linear_extrude(height=20) difference() {
            for(x=[-0.5,0.5],y=[-0.5,0.5]) {
                translate([esp32Holex*x,esp32HoleY*y])
                difference() {
                    circle(r=esp32HoleD);
                    circle(d=esp32HoleD*0.75,$fn=6);
                }
            }
            // some clearance is needed for the pins on the esp32 board
            translate([-esp32BoardX,esp32HoleY/2-7])
            square([esp32BoardX*2,5.5]);
    }

    // ithe rest of the chassis, too many hull()s for my liking...
    linear_extrude(height=7) {
        hull() {
            translate([esp32Holex/2,esp32HoleY/2+esp32Y])
            circle(r=esp32HoleD);
            translate([-esp32Holex/2,esp32HoleY/2+esp32Y])
            circle(r=esp32HoleD);
        }
        hull() {
            translate([esp32Holex/2,esp32HoleY/2+esp32Y])
            circle(r=esp32HoleD);
            translate([hBridgeHoleX/2,-hBridgeHoleY/2+hBridgeY])
            circle(r=hBridgeHoleD);
        }
        hull() {
            translate([-esp32Holex/2,esp32HoleY/2+esp32Y])
            circle(r=esp32HoleD);
            translate([-hBridgeHoleX/2,-hBridgeHoleY/2+hBridgeY])
            circle(r=hBridgeHoleD);
        }
        hull() {
            translate([hBridgeHoleX/2,hBridgeHoleY/2+hBridgeY])
            circle(r=hBridgeHoleD);
            translate([hBridgeHoleX/2,-hBridgeHoleY/2+hBridgeY])
            circle(r=hBridgeHoleD);
        }
        hull() {
            translate([-hBridgeHoleX/2,hBridgeHoleY/2+hBridgeY])
            circle(r=hBridgeHoleD);
            translate([-hBridgeHoleX/2,-hBridgeHoleY/2+hBridgeY])
            circle(r=hBridgeHoleD);
        }
        hull() {
            translate([-hBridgeHoleX/2,hBridgeHoleY/2+hBridgeY])
            circle(r=hBridgeHoleD);
            translate([-stepperMotorHoleW/2,stepperMotorY])
            scale([0.7,1]) circle(r=stepperMotorHoleD);
        }
        hull() {
            translate([hBridgeHoleX/2,hBridgeHoleY/2+hBridgeY])
            circle(r=hBridgeHoleD);
            translate([stepperMotorHoleW/2,stepperMotorY])
            scale([0.7,1]) circle(r=stepperMotorHoleD);
        }
    }
    difference() {
        union() {
            linear_extrude(height=16,convexity=10)
            hull() {
                translate([esp32Holex/2,esp32Y-esp32HoleY/2])
                circle(r=esp32HoleD);
                translate([-esp32Holex/2,esp32Y-esp32HoleY/2])
                circle(r=esp32HoleD);
            }
            linear_extrude(height=7,convexity=10)
            hull() {
                translate([0,esp32HoleY/2+esp32Y])
                circle(r=esp32HoleD);
                translate([0,-esp32HoleY/2+esp32Y])
                circle(r=esp32HoleD);
            }
        }
        translate([0,esp32Y-esp32HoleY/2-2,11])
        rotate([90,0,0])
        linear_extrude(height=4,convexity=10)
        scale([0.34,0.35,1])
        text("VL53L1X",halign="center",font="Liberation Sans:style=Bold");
        translate([0,esp32Y-esp32HoleY/2-2,6])
        rotate([90,0,0])
        linear_extrude(height=4,convexity=10)
        scale([0.4,0.35,1])
        text("ESP-32",halign="center",font="Liberation Sans:style=Bold");
        translate([0,esp32Y-esp32HoleY/2-2,2])
        rotate([90,0,0])
        linear_extrude(height=4,convexity=10)
        scale([0.75,0.3,1])
        text("lidar",halign="center",font="Liberation Serif:style=Bold Italic");
   }
}


module esp32(pos=[0,0,0]) translate(pos) {
    linear_extrude(height=1.5) {
        difference() {
            square([esp32BoardX,esp32BoardY],center=true);
            for (x=[-0.5,0.5],y=[-0.5,0.5]) {
                translate([esp32Holex*x,esp32HoleY*y])
                circle(d=esp32HoleD);
            }
        }
    }
}

module hBridge(pos=[0,0,0]) translate(pos) {
    linear_extrude(height=1.5) {
        difference() {
            square([hBridgeBoardX,hBridgeBoardY],center=true);
            for (x=[-0.5,0.5],y=[-0.5,0.5]) {
                translate([hBridgeHoleX*x,hBridgeHoleY*y])
                circle(d=hBridgeHoleD);
            }
        }
    }
}

module stepperMotor(pos=[0,0,0]) translate(pos) {
    linear_extrude(height=stepperMotorHeight) {
        circle(d=stepperMotorDiameter);
        translate([0,-stepperMotorDiameter/2+2])
        square([stepperMotorWireout,4],center=true);
    }
    translate([0,0,stepperMotorHeight-0.6])
    linear_extrude(height=0.6) {
        difference() {
            hull() {
                translate([-stepperMotorHoleW/2,0])
                circle(r=stepperMotorHoleD);
                translate([stepperMotorHoleW/2,0])
                circle(r=stepperMotorHoleD);
            }
            translate([-stepperMotorHoleW/2,0])
            circle(d=stepperMotorHoleD);
            translate([stepperMotorHoleW/2,0])
            circle(d=stepperMotorHoleD);
        }
    }
    translate([0,stepperMotorShaftOffset,stepperMotorHeight])
    cylinder(d=stepperMotorShaftBossDiameter,h=1.5);
    translate([0,stepperMotorShaftOffset,stepperMotorHeight+1.5])
    intersection() {
        cylinder(d=stepperMotorShaft,h=stepperMotorShaftLength);
        translate([-stepperMotorShaft/2,-stepperMotorShaftFlat/2,0])
        cube([stepperMotorShaft,stepperMotorShaftFlat,stepperMotorShaftLength]);
    }
    
}

module sensor(pos=[0,0,0],rot=90) 
    translate(pos) rotate([rot,0,0]) {
    linear_extrude(height=1.5) {
        square([sensorBoardW,sensorBoardH],center=true);
        difference() {
            hull() {
                translate([-sensorHoleW/2,0])
                circle(d=sensorHoleD*1.5);
                translate([sensorHoleW/2,0])
                circle(d=sensorHoleD*1.5);
            }
            translate([-sensorHoleW/2,0])
            circle(d=sensorHoleD);
            translate([sensorHoleW/2,0])
            circle(d=sensorHoleD);
        }
    }
}

