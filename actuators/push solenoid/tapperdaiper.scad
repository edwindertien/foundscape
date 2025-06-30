// luier
$fn=60;
difference(){
    cylinder(d=23,h=30);
    translate([0,0,2])cylinder(d=19,h=30);
    translate([-3,0,20])cube([6,30,22]);
}