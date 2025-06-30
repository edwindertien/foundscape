$fn=40;
hitter();
//plate();


module plate(){
difference(){
union(){cube([20,40,4.5]);
translate([10,0,0])cylinder(d=20,h=4.5);
    translate([0,13,4.5])cube([5,4,2.5]);
}
translate([10,0,0])cylinder(d=15,h=4.5);
translate([10,15,0])cylinder(d=5,h=4.5);
translate([10,30,0])cylinder(d=5.5,h=4.5);
translate([0.75,14.25,0])cube([3.5,1.5,8]);
}
}
module plate_small(){
difference(){
union(){cube([15,32.5,3]);
translate([7.5,0,0])cylinder(d=15,h=3);
    translate([0,13,3])cube([5,4,2.5]);
}
translate([7.5,0,0])cylinder(d=10,h=3);
translate([7.5,15,0])cylinder(d=5,h=3);
translate([7.5,25,0])cylinder(d=5.5,h=3);
translate([0.75,14.25,0])cube([3.5,1.5,6]);
}
}
module hitter(){
translate([0,-0,0]){difference(){
    cylinder(d=23,h=52);
    translate([0,0,10])cylinder(d=20,h=42.1);
    translate([-2,0,10])cube([4,20,12]);
}
translate([12.4,0,34]){
    
    difference(){
        union(){
            cylinder(d=3,h=18);
            translate([-1.5,-1.5,0])cube([1.5,3,18]);
        }
        cylinder(d=1,h=18);
    }

}
translate([-12.4,0,34]){
    
    difference(){
        union(){
            cylinder(d=3,h=18);
            translate([0,-1.5,0])cube([1.5,3,18]);
        }
        cylinder(d=1,h=18);
    }

}

}
}

module hitter_small(){
translate([0,-20,0]){difference(){
    cylinder(d=15,h=38);
    translate([0,0,10])cylinder(d=13.3,h=28.1);
    translate([-1.5,0,10])cube([3,10,10]);
}
translate([8.2,0,20]){
    
    difference(){
        union(){
            cylinder(d=3,h=18);
            translate([-1.5,-1.5,0])cube([1.5,3,18]);
        }
        cylinder(d=1,h=18);
    }

}
translate([-8.2,0,20]){
    
    difference(){
        union(){
            cylinder(d=3,h=18);
            translate([0,-1.5,0])cube([1.5,3,18]);
        }
        cylinder(d=1,h=18);
    }

}

}
}
