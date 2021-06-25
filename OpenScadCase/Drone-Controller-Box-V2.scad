//ESP8622-A9G-CASE

BoardLength		= 60;
BoardWidth		= 40;
BoardHeight		= 20;
BoardThickness 	= 1;

BoardScrewHoleRadius = 1;
ScrewHoleBodyRadius = 2;
ScrewHoleBodyHeight = 6;
ScrewHoleDistanceCenter = 2;
ScrewHoleBottomExtrusion = 1;

GPSLineLength 		= 120;
MobileLineLength 	= 150;

CaseWall    	= 1;
CaseTolerance 	= 1;
CaseLength  	= BoardLength + 2*CaseWall + CaseTolerance;
CaseWidth   	= BoardWidth + 2*CaseWall + CaseTolerance;
CaseHeight  	= 23;

Port1PosX 		= 7 + CaseWall + CaseTolerance;
Port1PosZ 		= CaseWall + ScrewHoleBodyHeight + BoardThickness;
Port1Width 		= 7;
Port1Height 	= 3;

Port2PosX 		= Port1PosX + Port1Width + 7 + CaseTolerance; //CaseWall + 21;
Port2PosZ 		= CaseWall + ScrewHoleBodyHeight + BoardThickness;
Port2Width 		= 8;
Port2Height 	= 6;

PowerSwitchHeight = 3;
PowerSwitchLength = 6;

ScrewPosXLeft = CaseWall+ScrewHoleDistanceCenter+CaseTolerance; //case tolerance only left for cable
ScrewPosXRight = CaseWall + CaseTolerance + BoardLength - ScrewHoleDistanceCenter;
ScrewPosY = CaseWall+ScrewHoleDistanceCenter+2*CaseTolerance;

//
$fn = 32;

//Box
difference(){
	union(){
		translate([-1,0,0]){
			cube([CaseLength+1,CaseWidth,CaseHeight]);
		}
	}
	translate([CaseWall,CaseWall,CaseWall]){ //inner
		translate([-1,0,0]){
			cube([CaseLength-(CaseWall*2)+1,CaseWidth-(CaseWall*2),CaseHeight]);
		}
	}
	//port1
	translate([Port1PosX,0,Port1PosZ+BoardThickness/3]){
		cube([Port1Width,CaseWall,Port1Height]);
	}
	//port2
	translate([Port2PosX,0,Port2PosZ+BoardThickness/3]){
		cube([Port2Width,CaseWall,Port2Height]);
	}
	//PowerSwitch
	translate([Port2PosX+Port2Width+9,0,Port2PosZ+BoardThickness/3+1.5]){
		cube([PowerSwitchLength,CaseWall,PowerSwitchHeight]);
	}
  	//ScrewHoles
	translate([ScrewPosXLeft,ScrewPosY-2*CaseTolerance,0]){
		cylinder(CaseWall+ScrewHoleBodyHeight,BoardScrewHoleRadius,BoardScrewHoleRadius);
		cylinder(ScrewHoleBottomExtrusion,2,2);
	}
	translate([ScrewPosXRight,ScrewPosY-2*CaseTolerance,0]){
		cylinder(CaseWall+ScrewHoleBodyHeight,BoardScrewHoleRadius,BoardScrewHoleRadius);
		cylinder(ScrewHoleBottomExtrusion,2,2);
	}
	translate([ScrewPosXLeft,CaseWidth-ScrewPosY+CaseWall,0]){
		cylinder(CaseWall+ScrewHoleBodyHeight,BoardScrewHoleRadius,BoardScrewHoleRadius);
		cylinder(ScrewHoleBottomExtrusion,2,2);
	}
	translate([ScrewPosXRight,CaseWidth-ScrewPosY+CaseWall,0]){
		cylinder(CaseWall+ScrewHoleBodyHeight,BoardScrewHoleRadius,BoardScrewHoleRadius);
		cylinder(ScrewHoleBottomExtrusion,2,2);
	}
}

//Edge
difference(){
	translate([0,0,CaseWall]){
		cube([CaseLength-CaseWall,CaseWidth-CaseWall,2*CaseWall]);
	}
	translate([CaseWall,CaseWall,2*CaseWall]){
		cube([CaseLength-3*CaseWall,CaseWidth-3*CaseWall,2*CaseWall]);
	}
}

//ScrewHoleBodies
difference(){
	translate([ScrewPosXLeft,ScrewPosY-2*CaseTolerance,CaseWall]){
		cylinder(ScrewHoleBodyHeight,ScrewHoleBodyRadius,ScrewHoleBodyRadius);
		cylinder(ScrewHoleBodyHeight/3,ScrewHoleBodyRadius+1,ScrewHoleBodyRadius+1);
	}
	translate([ScrewPosXLeft,ScrewPosY-2*CaseTolerance,CaseWall]){
		cylinder(CaseWall+ScrewHoleBodyHeight,BoardScrewHoleRadius,BoardScrewHoleRadius);
	}

}
difference(){
	translate([ScrewPosXRight,ScrewPosY-2*CaseTolerance,CaseWall]){
		cylinder(ScrewHoleBodyHeight,ScrewHoleBodyRadius,ScrewHoleBodyRadius);
	}
	translate([ScrewPosXRight,ScrewPosY-2*CaseTolerance,CaseWall]){
		cylinder(CaseWall+ScrewHoleBodyHeight,BoardScrewHoleRadius,BoardScrewHoleRadius);
	}
}
difference(){
	translate([ScrewPosXLeft,CaseWidth-ScrewPosY+CaseWall,CaseWall]){
		cylinder(ScrewHoleBodyHeight,ScrewHoleBodyRadius,ScrewHoleBodyRadius);
		cylinder(ScrewHoleBodyHeight/3,ScrewHoleBodyRadius+1.2,ScrewHoleBodyRadius+1.2);
	}
	translate([ScrewPosXLeft,CaseWidth-ScrewPosY+CaseWall,CaseWall]){
		cylinder(CaseWall+ScrewHoleBodyHeight,BoardScrewHoleRadius,BoardScrewHoleRadius);
	}
}
difference(){
	translate([ScrewPosXRight,CaseWidth-ScrewPosY+CaseWall,CaseWall]){
		cylinder(ScrewHoleBodyHeight,ScrewHoleBodyRadius,ScrewHoleBodyRadius);
		cylinder(ScrewHoleBodyHeight/3,ScrewHoleBodyRadius+1,ScrewHoleBodyRadius+1);
	}
	translate([ScrewPosXRight,CaseWidth-ScrewPosY+CaseWall,CaseWall]){
		cylinder(CaseWall+ScrewHoleBodyHeight,BoardScrewHoleRadius,BoardScrewHoleRadius);
	}
}

//Top
//translate([0,45,0]){
//union(){
//  cube([CaseLength,CaseWidth,CaseWall]);
//  translate([CaseWall,CaseWall,1]){ 
//	difference(){
//		cube([CaseLength-(CaseWall*2),CaseWidth-(CaseWall*2),CaseWall*2]);
//		translate([CaseWall,CaseWall,0]){ 
//			cube([CaseLength-(CaseWall*4),CaseWidth-(CaseWall*4),CaseWall*2]);
//			}
//		}
//    }
//  }
//}

//Port 1 closing
translate([8,-8,0]){
union(){
  cube([Port1Width+(CaseWall*2),Port1Height+(CaseWall*2),CaseWall]);
  translate([CaseWall,CaseWall,1]){ 
    cube([Port1Width,Port1Height,CaseWall]);
    }
  }
}

//Port 2 closing
translate([23,-10,0]){
union(){
  cube([Port2Width+(CaseWall*2),Port2Height+(CaseWall*2),CaseWall]);
  translate([CaseWall,CaseWall,1]){ 
    cube([Port2Width,Port2Height,CaseWall]);
    }
  }
}

//GPS ANTENNA HOLDER
GPSAntennaLength = 15;
GPSAntennaWidth  = 15;
GPSAntennaHeight = 6;

MobileAntennaLength = 35;
MobileAntennaWidth 	= 6;
MobileAntennaHeight = 2; //1 without solder

AntennaCaseWall = 1;

CoverInnerHeight = 1;

translate([64,15,0]){
	difference(){
		translate([1,-1,0]){ 
			cube([GPSAntennaLength+1*CaseWall+1,GPSAntennaWidth+1*CaseWall+1,GPSAntennaHeight+CaseWall]);
		}
		translate([CaseWall+1,0,CaseWall]){ 
			cube([GPSAntennaLength-1+1,GPSAntennaWidth-1+1,GPSAntennaHeight+CaseWall]);
		}
		translate([1,(GPSAntennaWidth)/2-0.5,CaseWall]){ 
			cube([CaseWall,CaseWall,GPSAntennaHeight]);
		}
	}
}

translate([65,0,0]){
	difference(){
		cube([MobileAntennaLength+2*CaseWall,MobileAntennaWidth+2*CaseWall,MobileAntennaHeight+CaseWall]);
		translate([1,1,1]){ 
			cube([MobileAntennaLength,MobileAntennaWidth,MobileAntennaHeight+CaseWall]);
		}
		translate([MobileAntennaLength+CaseWall,(MobileAntennaWidth+2*CaseWall)/2-1-0.5,CaseWall]){ 
			cube([CaseWall,CaseWall,MobileAntennaHeight]);
		}
	}
}

//MPU9250 ADDITION
MPU9250Lenght = 21;//20.5;
MPU9250Width = 15;//14.3;

MPU9250ScrewHoleRadius = 1;//real 1.5;
MPU9250ScrewHoleDistanceCenter = 2.5;

MPU9250FixWidth = 5;
//translate([0,0,2]){

module right_triangle(side1,side2,corner_radius,triangle_height){
  translate([corner_radius,corner_radius,0]){  
    hull(){  
    cylinder(r=corner_radius,h=triangle_height);
      translate([side1 - corner_radius * 2,0,0])cylinder(r=corner_radius,h=triangle_height);
          translate([0,side2 - corner_radius * 2,0])cylinder(r=corner_radius,h=triangle_height);  
    }
  }
    
}

translate([0,0,1]){  
difference(){
	translate([CaseWall+CaseTolerance+5,CaseWidth-CaseWall-MPU9250FixWidth,CaseWall+ScrewHoleBodyHeight+BoardThickness+4+2]){
		cube([MPU9250Lenght+1,MPU9250FixWidth,1]);
		translate([MPU9250Lenght+1,5,0]) {
			rotate(a=[90,90,-90]) {
				//cylinder(MPU9250Lenght,MPU9250FixWidth,MPU9250FixWidth,$fn = 4);
				right_triangle(MPU9250FixWidth,MPU9250FixWidth,0.00001,MPU9250Lenght+1);
			}
		}
		
	}
	translate([CaseWall+CaseTolerance+5+MPU9250ScrewHoleDistanceCenter,CaseWidth-CaseWall-MPU9250FixWidth+MPU9250ScrewHoleDistanceCenter,CaseWall+ScrewHoleBodyHeight+BoardThickness+2]){
		cylinder(MPU9250FixWidth,MPU9250ScrewHoleRadius,MPU9250ScrewHoleRadius);
		translate([-MPU9250ScrewHoleDistanceCenter+MPU9250ScrewHoleDistanceCenter-MPU9250ScrewHoleRadius-0.5,-MPU9250ScrewHoleDistanceCenter+MPU9250ScrewHoleDistanceCenter-MPU9250ScrewHoleRadius-0.5,0]) {
			cube([4,4,3]);
		}
	}
	translate([CaseWall+CaseTolerance+5-MPU9250ScrewHoleDistanceCenter+MPU9250Lenght,CaseWidth-CaseWall-MPU9250FixWidth+MPU9250ScrewHoleDistanceCenter,CaseWall+ScrewHoleBodyHeight+BoardThickness+2]){
		cylinder(MPU9250FixWidth,MPU9250ScrewHoleRadius,MPU9250ScrewHoleRadius);
		translate([-MPU9250ScrewHoleDistanceCenter+MPU9250ScrewHoleDistanceCenter-MPU9250ScrewHoleRadius-0.5,-MPU9250ScrewHoleDistanceCenter+MPU9250ScrewHoleDistanceCenter-MPU9250ScrewHoleRadius-0.5,0]) {
			cube([4,4,3]);
		}
	}
}
	translate([CaseWall+CaseTolerance+5+MPU9250ScrewHoleDistanceCenter,CaseWidth-CaseWall-MPU9250FixWidth+MPU9250ScrewHoleDistanceCenter,CaseWall+ScrewHoleBodyHeight+BoardThickness]){
}
}

//}
//Openlog Datalogger
OpenlogLenght = 19.7;
OpenlogWidth = 15.8;
OpenlogHeight = 5;

//Battery
BatteryCaseTolerance = 1;
BatteryLength = 50 + BatteryCaseTolerance;
BatteryWidth = 34 + BatteryCaseTolerance;
BatteryHeight = 5;
BatteryEnclosureHeight = BatteryHeight + BatteryCaseTolerance;
BatteryEnclosureWall = 1;
BatteryEnclosureLength = BatteryLength+2*BatteryEnclosureWall;
BatteryEnclosureWidth = BatteryWidth+2*BatteryEnclosureWall;
BatteryEnclosurePosX = (CaseLength-BatteryEnclosureLength)/2 + 1 /*offset*/;
BatteryEnclosurePosY = (CaseWidth-BatteryEnclosureWidth)/2 - CaseTolerance - 2 /*offset*/;
//ScrewHoleBodyHeight = 5;

difference(){
	translate([BatteryEnclosurePosX,BatteryEnclosurePosY,CaseWall]){
		cube([BatteryEnclosureLength,BatteryEnclosureWidth,BatteryEnclosureHeight]);
	}
	translate([BatteryEnclosurePosX+BatteryEnclosureWall,BatteryEnclosurePosY+BatteryEnclosureWall,CaseWall]){
		cube([BatteryEnclosureLength-(BatteryEnclosureWall*2),BatteryEnclosureWidth-(BatteryEnclosureWall*2),CaseHeight]);
	}
	translate([BatteryEnclosurePosX,BatteryEnclosurePosY+5*BatteryEnclosureWall,CaseWall]){
		cube([BatteryEnclosureWall,8*BatteryEnclosureWall,5*BatteryEnclosureWall]);
	}
}

////PMW3901 Optical Flow Sensor 
//
//PMW3901Lenght = 33;
//PMW3901Width = 25;
//PMW3901Height = 1; //only board
////sensor back
//PMW3901Lenght2 = PMW3901Lenght;
//PMW3901Width2 = 7;
////sensor head
//PMW3901Lenght3 = 17;
//PMW3901Width3 = 12;
//PMW3901Height3 = 4; //board with cam
//
//PMW3901ScrewHoleRadius = 1.5;
//PMW3901ScrewHoleDistanceCenterX = 1;
//PMW3901ScrewHoleDistanceCenterY = 2;
//
//PMW3901SensorLength = 5 + 0.5;
//PMW3901SensorWidth = 5 + 0.5;
//PMW3901SensorDistanceMidX = 3;
//PMW3901SensorDistanceMidY = 6;
//PMW3901SensorHolderWall = 0.4;
//
//translate([CaseLength+10,0,0]){
//	difference(){
//		cube([PMW3901Width-PMW3901Width3+CaseWall,PMW3901Lenght2+2*CaseWall,PMW3901Height3]);
//		translate([0,CaseWall,1]){ 
//			cube([PMW3901Width-PMW3901Width3,PMW3901Lenght2,PMW3901Height3]);
//		}
//		translate([PMW3901Width-PMW3901Width3,(PMW3901Lenght-PMW3901Lenght3)/2+CaseWall,0]){
//			cube([CaseWall,PMW3901Lenght3,PMW3901Height3]);
//		}
//
//	}
//	translate([PMW3901Width-PMW3901Width3,(PMW3901Lenght-PMW3901Lenght3)/2,0]){
//		difference(){
//			cube([PMW3901Width3+2*CaseWall,PMW3901Lenght3+2*CaseWall,PMW3901Height3]);
//			translate([0,CaseWall,1]){ 
//				cube([PMW3901Width3+CaseWall,PMW3901Lenght3,PMW3901Height3]);
//			}
//			translate([0,1,1]){ 
//				cube([CaseWall,PMW3901Lenght3,PMW3901Height3]);
//			}
//		}
//		//sensor holer
//		translate([PMW3901Width3-PMW3901SensorDistanceMidY,PMW3901SensorDistanceMidX,-1]){
//			difference(){
//				cube([PMW3901SensorWidth+2*CaseWall,PMW3901SensorLength+2*CaseWall,PMW3901Height3]);
//				translate([CaseWall,CaseWall,0]){ 
//					cube([PMW3901SensorWidth+CaseWall,PMW3901SensorLength,PMW3901Height3]);
//				}
//			}
//		}
//	}
//}
//
////Slide cover
//
//SlideCoverLayerHeight = 1;
//
////BoardHeight
//translate([0,CaseWidth+20,0]){
//union(){
//  cube([CaseLength-(CaseWall/2),CaseWidth-2*(CaseWall/2),SlideCoverLayerHeight]);
//  translate([CaseWall/2,CaseWall/2,SlideCoverLayerHeight]){ 
//    cube([CaseLength-CaseWall,CaseWidth-(CaseWall*2),SlideCoverLayerHeight]);
//    }
//  }
//}
//

//Slide cover insert

//left
translate([CaseLength,0,CaseHeight+2*CaseWall]) {
	rotate(a=[90,90,-90]) {
		//cylinder(MPU9250Lenght,MPU9250FixWidth,MPU9250FixWidth,$fn = 4);
		right_triangle(MPU9250FixWidth,MPU9250FixWidth-3,0.00001,CaseLength+1);
	}
}
translate([-1,0,CaseHeight+CaseWall]) {
	rotate(a=[270,90,-90]) {
		//cylinder(MPU9250Lenght,MPU9250FixWidth,MPU9250FixWidth,$fn = 4);
		right_triangle(1,1,0.00001,CaseLength+1);
	}
	cube([CaseLength+1,1,SlideCoverLayerHeight]);
}

//right
translate([CaseLength-CaseWall,CaseWidth,0]) {
	rotate(a=[0,0,180]) {
		translate([CaseLength,0,CaseHeight+2*CaseWall]) {
			rotate(a=[90,90,-90]) {
				//cylinder(MPU9250Lenght,MPU9250FixWidth,MPU9250FixWidth,$fn = 4);
				right_triangle(MPU9250FixWidth,MPU9250FixWidth-3,0.00001,CaseLength+1);
			}
		}
		translate([-1,0,CaseHeight+CaseWall]) {
			rotate(a=[270,90,-90]) {
				//cylinder(MPU9250Lenght,MPU9250FixWidth,MPU9250FixWidth,$fn = 4);
				right_triangle(1,1,0.00001,CaseLength+1);
			}
			cube([CaseLength+1,1,SlideCoverLayerHeight]);
		}
	}
}

translate([-1,0,CaseHeight]) {
	cube([CaseWall,CaseWidth,2*CaseWall]);
}

//Slide cover
translate([-1,CaseWidth+5,-CaseHeight]) {
	translate([0,CaseWall,CaseHeight+CaseWall]) {
		cube([CaseLength,CaseWidth-2*CaseWall,CaseWall]);
	}
	translate([0,CaseWall,CaseHeight]) {
		cube([CaseLength,CaseWidth-2*CaseWall,CaseWall]);
	}
	translate([0,CaseWall,CaseHeight]) {
		rotate(a=[90,270,-270]) {
			//cylinder(MPU9250Lenght,MPU9250FixWidth,MPU9250FixWidth,$fn = 4);
			right_triangle(1,1,0.00001,CaseLength);
		}
	}
	translate([0,CaseWidth-1,CaseHeight]) {
		rotate(a=[90,360,-270]) {
			//cylinder(MPU9250Lenght,MPU9250FixWidth,MPU9250FixWidth,$fn = 4);
			right_triangle(1,1,0.00001,CaseLength);
		}
	}
}