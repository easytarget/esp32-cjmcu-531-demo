// Lidar Base
// A bit overdone, but works OK. 
// Wayyy too many parameters, but Boards are resizable
//  and can be moved relative to each other.

$fn=90;

fitting=!true;

stand();

mcuY = -40;
drvY = 5;
motY = 40;
// tofs prefix used for flight sensor


if (fitting) 
    mount(pos=[0,motY+motsoffset-3.5,50],rot=-90);
else
    mount([0,-3.5,26],-90); 
    
if (fitting) {
    color("darkgreen",0.8) mcu(pos=[0,mcuY,20]);
    color("green",0.8) drv(pos=[0,drvY,12]);
    color("grey",0.8) mot(pos=[0,motY,0]);
    color("midnightblue",0.8) tof(pos=[0,motY+motsoffset+4.5,50.7],rot=95);
}

// Parts & Dimensions

mcuholeX=23.5;
mcuholeY=47;
mcuholeD=3;
mcuboardX=28.7;
mcuboardY=51.7;

drvholeX=29.5;
drvholeY=26.5;
drvholeD=3;
drvboardX=34.5;
drvboardY=32;

motdia=28;
mothigh=19.25;
motwireout=17.5;
motholeW=35;
motholeD=4.2;
motshaft=5;
motsflat=3;
motsoffset=7.5;
motsbossD=9.3;
motsl=8.5; 

tofsholeW=20;
tofsholeD=3.5;
tofsboardH=12.5;
tofsboardW=15.8;



module mount(pos=[0,0,0],rot=0) translate(pos) rotate([rot,0,0]) {
    translate([0,0,-1]) rotate([5,0,0])
    linear_extrude(height=7.5) {
        translate([-tofsholeW/2,0])
        difference() {
            circle(d=tofsholeD*1.5);
            rotate(30) circle(d=tofsholeD*0.75,$fn=6);
        }
        translate([tofsholeW/2,0])
        difference() {
            circle(d=tofsholeD*1.5);
            rotate(30) circle(d=tofsholeD*0.75,$fn=6);
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
            cylinder(d=motshaft+0.3,h=7,center=true);
            cube([motshaft+1,motsflat,7],center=true);
        }
        translate([0,13,3.5])
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
    

module stand() {
    // motor pins
    translate([0,motY,0])
    linear_extrude(height=mothigh-0.6) {
        for(x=[-0.5,0.5]) {
            translate([motholeW*x,0])
            difference() {
                scale([0.7,1]) circle(r=motholeD);
                rotate(30) circle(d=motholeD*0.7,$fn=6);
            }
        }
    }

    // driver pins
    translate([0,drvY,0])
    linear_extrude(height=12) {
        for(x=[-0.5,0.5],y=[-0.5,0.5]) {
            translate([drvholeX*x,drvholeY*y])
            difference() {
                circle(r=drvholeD);
                circle(d=drvholeD*0.75,$fn=6);
            }
        }
    }
        
    // mcu pins
    translate([0,mcuY,0]) 
    linear_extrude(height=20) difference() {
            for(x=[-0.5,0.5],y=[-0.5,0.5]) {
                translate([mcuholeX*x,mcuholeY*y])
                difference() {
                    circle(r=mcuholeD);
                    circle(d=mcuholeD*0.75,$fn=6);
                }
            }
            translate([-mcuboardX,mcuholeY/2-7])
            square([mcuboardX*2,5.5]);
    }

    // chassis, hull city
    linear_extrude(height=7) {
        hull() {
            translate([mcuholeX/2,mcuholeY/2+mcuY])
            circle(r=mcuholeD);
            translate([-mcuholeX/2,mcuholeY/2+mcuY])
            circle(r=mcuholeD);
        }
        hull() {
            translate([mcuholeX/2,mcuholeY/2+mcuY])
            circle(r=mcuholeD);
            translate([drvholeX/2,-drvholeY/2+drvY])
            circle(r=drvholeD);
        }
        hull() {
            translate([-mcuholeX/2,mcuholeY/2+mcuY])
            circle(r=mcuholeD);
            translate([-drvholeX/2,-drvholeY/2+drvY])
            circle(r=drvholeD);
        }
        hull() {
            translate([drvholeX/2,drvholeY/2+drvY])
            circle(r=drvholeD);
            translate([drvholeX/2,-drvholeY/2+drvY])
            circle(r=drvholeD);
        }
        hull() {
            translate([-drvholeX/2,drvholeY/2+drvY])
            circle(r=drvholeD);
            translate([-drvholeX/2,-drvholeY/2+drvY])
            circle(r=drvholeD);
        }
        hull() {
            translate([-drvholeX/2,drvholeY/2+drvY])
            circle(r=drvholeD);
            translate([-motholeW/2,motY])
            scale([0.7,1]) circle(r=motholeD);
        }
        hull() {
            translate([drvholeX/2,drvholeY/2+drvY])
            circle(r=drvholeD);
            translate([motholeW/2,motY])
            scale([0.7,1]) circle(r=motholeD);
        }
    }
    difference() {
        union() {
            linear_extrude(height=16,convexity=10)
            hull() {
                translate([mcuholeX/2,mcuY-mcuholeY/2])
                circle(r=mcuholeD);
                translate([-mcuholeX/2,mcuY-mcuholeY/2])
                circle(r=mcuholeD);
            }
            linear_extrude(height=7,convexity=10)
            hull() {
                translate([0,mcuholeY/2+mcuY])
                circle(r=mcuholeD);
                translate([0,-mcuholeY/2+mcuY])
                circle(r=mcuholeD);
            }
        }
        translate([0,mcuY-mcuholeY/2-2,11])
        rotate([90,0,0])
        linear_extrude(height=4,convexity=10)
        scale([0.34,0.35,1])
        text("VL53L1X",halign="center",font="Liberation Sans:style=Bold");
        translate([0,mcuY-mcuholeY/2-2,6])
        rotate([90,0,0])
        linear_extrude(height=4,convexity=10)
        scale([0.4,0.35,1])
        text("ESP-32",halign="center",font="Liberation Sans:style=Bold");
        translate([0,mcuY-mcuholeY/2-2,2])
        rotate([90,0,0])
        linear_extrude(height=4,convexity=10)
        scale([0.75,0.3,1])
        text("lidar",halign="center",font="Liberation Serif:style=Bold Italic");
   }
}


module mcu(pos=[0,0,0]) translate(pos) {
    linear_extrude(height=1.5) {
        difference() {
            square([mcuboardX,mcuboardY],center=true);
            for (x=[-0.5,0.5],y=[-0.5,0.5]) {
                translate([mcuholeX*x,mcuholeY*y])
                circle(d=mcuholeD);
            }
        }
    }
}

module drv(pos=[0,0,0]) translate(pos) {
    linear_extrude(height=1.5) {
        difference() {
            square([drvboardX,drvboardY],center=true);
            for (x=[-0.5,0.5],y=[-0.5,0.5]) {
                translate([drvholeX*x,drvholeY*y])
                circle(d=drvholeD);
            }
        }
    }
}

module mot(pos=[0,0,0]) translate(pos) {
    linear_extrude(height=mothigh) {
        circle(d=motdia);
        translate([0,-motdia/2+2])
        square([motwireout,4],center=true);
    }
    translate([0,0,mothigh-0.6])
    linear_extrude(height=0.6) {
        difference() {
            hull() {
                translate([-motholeW/2,0])
                circle(r=motholeD);
                translate([motholeW/2,0])
                circle(r=motholeD);
            }
            translate([-motholeW/2,0])
            circle(d=motholeD);
            translate([motholeW/2,0])
            circle(d=motholeD);
        }
    }
    translate([0,motsoffset,mothigh])
    cylinder(d=motsbossD,h=1.5);
    translate([0,motsoffset,mothigh+1.5])
    intersection() {
        cylinder(d=motshaft,h=motsl);
        translate([-motshaft/2,-motsflat/2,0])
        cube([motshaft,motsflat,motsl]);
    }
    
}

module tof(pos=[0,0,0],rot=90) 
    translate(pos) rotate([rot,0,0]) {
    linear_extrude(height=1.5) {
        square([tofsboardW,tofsboardH],center=true);
        difference() {
            hull() {
                translate([-tofsholeW/2,0])
                circle(d=tofsholeD*1.5);
                translate([tofsholeW/2,0])
                circle(d=tofsholeD*1.5);
            }
            translate([-tofsholeW/2,0])
            circle(d=tofsholeD);
            translate([tofsholeW/2,0])
            circle(d=tofsholeD);
        }
    }
}

