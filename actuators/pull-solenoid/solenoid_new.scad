$fn=40;
//hitter();
//mirror()plate();
//mirror()plate_small();
hitter_small();
//link();
//link_small();

module link(){ difference(){
    union(){
cube([5,14.5,2]);
        translate([5,4,0])cube([5,10,2]);
translate([2.5,0,0])cylinder(d=5,h=2);
        translate([2.5,14.5,0])cylinder(d=5,h=2);
translate([2.5,12,0])cube([32.5,5,2]);
translate([35,14.5,0])cylinder(d=5,h=2);

    }
translate([2.5,0,0])cylinder(d=1.6,h=2);
translate([35,14.5,0])cylinder(d=1.6,h=2); 
 translate([1,8-1.5,0])cube([1.8,3.2,4]);
 translate([4,8-1.5,0])cube([1.8,3.2,4]);  
 translate([7,8-1.5,0])cube([1.8,3.2,4]);     
}}

module link_small(){ difference(){
    union(){
cube([5,10,1.4]);
        translate([5,2.8,0])cube([5,8,1.4]);
translate([2.5,0,0])cylinder(d=5,h=1.4);
        translate([2.5,10,0])cylinder(d=5,h=1.4);
translate([2.5,7.5,0])cube([28.5,5,1.4]);
translate([31,10,0])cylinder(d=5,h=1.4);

    }
translate([2.5,0,0])cylinder(d=1.6,h=2);
translate([31,10,0])cylinder(d=1.6,h=2); 
 translate([1,5.5-1.5,0])cube([1.8,3.2,4]);
 translate([4,5.5-1.5,0])cube([1.8,3.2,4]);  
 translate([7,5.5-1.5,0])cube([1.8,3.2,4]);     
}}
module plate(){
difference(){
union(){cube([20,40,4.5]);
translate([10,0,0])cylinder(d=20,h=4.5);
    translate([10,40,0])cylinder(d=20,h=4.5);
    translate([25,15,0])cylinder(d=20,h=4.5);
    translate([20,5,0])cube([5,20,4.5]);
    translate([0,13,4.5])cube([5,4,2.5]);
}
translate([10,0,0])cylinder(d=15,h=4.5);
translate([14,15,0])cylinder(d=4,h=4.5);
translate([10,40,0])cylinder(d=9.6,h=4.5);
 translate([25,15,0])cylinder(d=11.2,h=4.5);
translate([0.75,14.25,0])cube([3.5,1.5,8]);
}
}
module plate_small(){
difference(){
union(){cube([15,35,3]);
translate([7.5,0,0])cylinder(d=15,h=3);
    translate([7.5,35,0])cylinder(d=15,h=3);
    translate([20,15,0])cylinder(d=15,h=3);
    translate([15,7.5,0])cube([5,15,3]);
    translate([0,13,3])cube([5,4,2.5]);
}
translate([7.5,0,0])cylinder(d=10,h=3);
translate([10,15,0])cylinder(d=4,h=4.5);
translate([7.5,35,0])cylinder(d=9.6,h=4.5);
 translate([20,15,0])cylinder(d=11.2,h=4.5);
translate([1,14.25,0])cube([3,1.4,8]);
}
}
module hitter() {
  translate([0, -0, 0]) {
    difference() {
      union() {
        cylinder(d = 23, h = 52);
        translate([11, 0, 34]) {
          difference() {
            union() {
              translate([0, -3.3, 0])cube([6, 2, 18]);
              translate([0, 1.3, 0])cube([6, 2, 18]);
              translate([0, 3.3, 0])rotate([90, 0, 0])cylinder(d = 12, h = 2);
              translate([0, -1.3, 0])rotate([90, 0, 0])cylinder(d = 12, h = 2);
            }
            translate([3.5, 10, 15])rotate([90, 0, 0])cylinder(d = 2.2, h = 18);
            translate([3.5, 10, 11])rotate([90, 0, 0])cylinder(d = 2.2, h = 18);
            translate([3.5, 10, 7])rotate([90, 0, 0])cylinder(d = 2.2, h = 18);
          }
        }
      }
      translate([0, 0, 10])cylinder(d = 20, h = 42.1);
      translate([-2, 0, 10])cube([4, 20, 12]);
    }
  }
}

module hitter_small() {
  translate([0, -20, 0]) {
    difference() {
        union() {
      cylinder(d = 16, h = 38);
                translate([6.5, 0, 20]) {
          difference() {
            union() {
              translate([0, -3, 0])cube([6, 2, 18]);
              translate([0, 1, 0])cube([6, 2, 18]);
              translate([0, 3, 0])rotate([90, 0, 0])cylinder(d = 12, h = 2);
              translate([0, -1, 0])rotate([90, 0, 0])cylinder(d = 12, h = 2);
            }
            translate([3.8, 10, 15])rotate([90, 0, 0])cylinder(d = 2.2, h = 18);
            translate([3.8, 10, 11])rotate([90, 0, 0])cylinder(d = 2.2, h = 18);
            translate([3.8, 10, 7])rotate([90, 0, 0])cylinder(d = 2.2, h = 18);
          }
        }    
            
        }
      translate([0, 0, 10])cylinder(d = 13.8, h = 28.1);
      translate([-1.5, 0, 10])cube([3, 10, 10]);
    }
  }
}

