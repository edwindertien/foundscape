$fn=40;


difference(){
    hull(){
        cylinder(d=20,h=5);
        translate([0,18,0])cylinder(d=20,h=5);
        translate([0,-18,0])cylinder(d=10,h=5);
        translate([0,-28,0])cylinder(d=10,h=5);
    }
    cylinder(d=14.3,h=6);
    translate([0,18,0])cylinder(d=16,h=6);
            translate([0,-18,0])cylinder(d=4.8,h=6);
        translate([0,-28,0])cylinder(d=4.8,h=6);
}