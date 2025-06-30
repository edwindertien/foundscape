$fn = 40;


width = 102;
depth = 77;
wall = 3;
corner = 7;

top = 2;
rim = 1;

mount_y = 60;
mount_x = 80;
mount_d = 6;
mount_h = 3;
mount_drill = 2.5;

height = 27+top+mount_h;
//box();
//translate([0,0,height+wall-top])lid();
projection(cut = false) lid();

module box() {difference() {
  union() {
    difference() {
      hull() {
        translate([0, 0, 0])cylinder(d = corner, h = height + wall);
        translate([width, depth, 0])cylinder(d = corner, h = height + wall);
        translate([0, depth, 0])cylinder(d = corner, h = height + wall);
        translate([width, 0, 0])cylinder(d = corner, h = height + wall);
      }
      translate([0, 0, wall])cube([width, depth, height + 0.01]);
    }
    translate([0, 0, 0])cylinder(d = 5, h = height + wall);
    translate([width, depth, 0])cylinder(d = 5, h = height + wall);
    translate([0, depth, 0])cylinder(d = 5, h = height + wall);
    translate([width, 0, 0])cylinder(d = 5, h = height + wall);
    // mount pole:
    translate([width/2-mount_x/2, 3 + depth/2-mount_y/2, 0])cylinder(d = mount_d, h = mount_h+wall);
    translate([width/2+mount_x/2, 3 +depth/2-mount_y/2, 0])cylinder(d = mount_d, h = mount_h+wall);
    translate([width/2+mount_x/2, 3 +depth/2+mount_y/2, 0])cylinder(d = mount_d, h = mount_h+wall);
    translate([width/2-mount_x/2, 3 +depth/2+mount_y/2, 0])cylinder(d = mount_d, h = mount_h+wall);
    
  }
  // pcb mount drill holes:
      // mount pole:
    translate([width/2-mount_x/2, 3 +depth/2-mount_y/2, wall-0.5])cylinder(d = mount_drill, h = mount_h+wall);
    translate([width/2+mount_x/2, 3 +depth/2-mount_y/2, wall-0.5])cylinder(d = mount_drill, h = mount_h+wall);
    translate([width/2+mount_x/2, 3 +depth/2+mount_y/2, wall-0.5])cylinder(d = mount_drill, h = mount_h+wall);
    translate([width/2-mount_x/2, 3 +depth/2+mount_y/2, wall-0.5])cylinder(d = mount_drill, h = mount_h+wall);
  // lid holes
  translate([0, 0, height - 15])cylinder(d = 2.5, h = height + wall);
  translate([width, depth, height - 15])cylinder(d = 2.5, h = height + wall);
  translate([0, depth, height - 15])cylinder(d = 2.5, h = height + wall);
  translate([width, 0, height - 15])cylinder(d = 2.5, h = height + wall);
  // lid
  hull() {
    translate([0, 0, height + wall - top])cylinder(d = corner - 2*rim, h = height + wall);
    translate([width, depth, height + wall - top])cylinder(d = corner - 2*rim, h = height + wall);
    translate([0, depth, height + wall - top])cylinder(d = corner - 2*rim, h = height + wall);
    translate([width, 0, height + wall - top])cylinder(d = corner - 2*rim, h = height + wall);
  }
  // side holes
  translate([55,-10,wall+mount_h + 2])cube([32,20,16]);
  translate([15,depth-10,wall+mount_h + 2])cube([73,20,8]);
}}


module lid(){
    difference(){
      hull() {
    translate([0, 0, 0])cylinder(d = corner - 2*rim, h = top);
    translate([width, depth, 0])cylinder(d = corner - 2*rim,  h = top);
    translate([0, depth, 0])cylinder(d = corner - 2*rim, h = top);
    translate([width, 0, 0])cylinder(d = corner - 2*rim,  h = top);
  }
    translate([0, 0, 0])cylinder(d = 2.5, h = height + wall);
  translate([width, depth, 0])cylinder(d = 2.5, h = height + wall);
  translate([0, depth, 0])cylinder(d = 2.5, h = height + wall);
  translate([width, 0, 0])cylinder(d = 2.5, h = height + wall);
  }
  }