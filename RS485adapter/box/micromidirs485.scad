$fn = 40;
//box();
lid();

module box() difference(){
    hull(){
        translate([0,65,0])cylinder(d=3,h=20);
        translate([20,65,0])cylinder(d=3,h=20);
        translate([20,0,0])cylinder(d=3,h=20);
        translate([0,0,0])cylinder(d=3,h=20);
    }
    translate([0,0,2])cube([20,65,22]);
    translate([1.5,-5,6])cube([17,10,15]);
    translate([1.5,-5,6])cube([17,10,15]);
    translate([10-5.5,66,13.1])cube([11,4,7]);
    translate([10-4,64,15])cube([8,4,3]);
}
         

module lid() translate([0,0,30]) union(){ 
   difference(){
      hull(){
        translate([0,65,0])cylinder(d=3,h=4);
        translate([20,65,0])cylinder(d=3,h=4);
        translate([20,0,0])cylinder(d=3,h=4);
        translate([0,0,0])cylinder(d=3,h=4);
    }
    translate([0,0,2])cube([20,65,10]);
  }

    translate([0,0,2])cube([1.5,65,3]);
    translate([20-1.5,0,2])cube([1.5,65,3]);
}