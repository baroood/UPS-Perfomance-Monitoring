CREATE DATABASE IF NOT EXISTS ESW;
use ESW;

drop table if exists data_stored;
create table data_stored(
	Time_stamp VARCHAR(30) , 
	Temperature VARCHAR(30) , 
	Humidity VARCHAR(30) , 
	PowerIN VARCHAR(30) , 
	PowerOUT VARCHAR(30), 
	EnergyIN VARCHAR(30), 
	EnergyOUT VARCHAR(30) , 
	EnergyDiff VARCHAR(30), 
 	PRIMARY KEY (Time_stamp)
 );

