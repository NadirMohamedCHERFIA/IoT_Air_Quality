const express=require('express');
const mysql=require('mysql');
const cors=require('cors');
const mqtt=require('mqtt');
const { json } = require('express/lib/response');

//const connectWithREtry = () =>{
///Create connection 
const db=mysql.createConnection({
    host:"localhost",
    user:'root',
    password:'Cherfianadir2022@',
    database: 'iot_air_quality'
});
//connect
db.connect((err)=>{
    if(err){
        throw err;
        setTimeout(connectWithREtry,5000)
    }
    console.log('mysql connected ....');
});
//}
options={
    username:"iot_enst",
    password:"cherfianadir",
    clean:true}
var client = mqtt.connect("mqtt://localhost",options)
client.on("connect",function(){	
    console.log("client1:mosquitto connected");
})
client.on("error",function(error){
    console.log("Can't connect" + error);
    process.exit(1)});
    var topic1="esp32/BV1.1/jsonFormatedData1";
    console.log("subscribing t/o topic");
    client.subscribe(topic1,{qos:1});
    client.on('message',function(topic1, message, packet){
        global.message1=message;
        console.log("message is "+ message1);
        console.log("topic is "+ topic1);
    });
    var client2 = mqtt.connect("mqtt://localhost",options)
    client2.on("connect",function(){	
        console.log("client2:mosquitto connected");
    })
    client2.on("error",function(error){
        console.log("Can't connect" + error);
        process.exit(1)});
        var topic2="esp32/BV1.1/jsonFormatedData2";
        console.log("subscribing t/o topic");
        client2.subscribe(topic2,{qos:1});
        client2.on('message',function(topic2, message, packet){
        const jsonFormatedData2=JSON.parse(message);
        const jsonFormatedData=JSON.parse(message1);
        var query1='INSERT INTO air_quality SET '+'TEMPERATURE_C='+jsonFormatedData.temperature+',HUMIDITY_percentage='+jsonFormatedData.humidity+',ALTITUDE_m='+jsonFormatedData.altitude+',PRESSURE_hPa='+jsonFormatedData.pressure+',PM10='+jsonFormatedData.PM10+',PM25='+jsonFormatedData.PM25+',PM100='+jsonFormatedData.PM100+
        ',P03um_perdecilitre='+jsonFormatedData.P03um+',P05um_perdecilitre='+jsonFormatedData.P05um+',P10um_perdecilitre='+jsonFormatedData.P10um+',P25um_perdecilitre='+jsonFormatedData.P25um+',P50um_perdecilitre='+jsonFormatedData.P50um+',P100um_perdecilitre='+jsonFormatedData.P100um+
        ",CO2_ppm="+jsonFormatedData2.CO2+",TVOC_ppb="+jsonFormatedData2.TVOC+",AIR_QUALITY_ppm="+jsonFormatedData2.AIR_QUALITY+",GAS_RESISTANCE_KOhms="+jsonFormatedData2.GAS_RESISTANCE;
            console.log("message is "+ message);
            console.log("topic is "+ topic2);
            console.log(query1);
            db.query(query1,(err,result)=>{
                if(err) throw err;
                console.log(result);
            });
        });
    
        
        
 
//connectWithREtry();

const app = express();
app.use(cors({
    origin:'*',
}));
//db creation 
app.get('/createdb',(req,res)=>{
    let sql = 'CREATE DATABASE iot_air_quality';
    db.query(sql,(err,result)=>{
        if(err) throw err;
        console.log(result);
        res.send('database created .....');
    });
});
//db creation 
app.get('/createdb',(req,res)=>{
    let sql = 'CREATE DATABASE iot_air_quality';
    db.query(sql,(err,result)=>{
        if(err) throw err;
        console.log(result);
        res.send('database created .....');
    });
});
//create table
app.get('/createtable',(req,res)=>{
    let sql ='CREATE TABLE air_quality(id INT AUTO_INCREMENT PRIMARY KEY ,TEMPERATURE_C VARCHAR(20),HUMIDITY_percentage VARCHAR(20),ALTITUDE_m VARCHAR(20),PRESSURE_hPa VARCHAR(20),PM10 VARCHAR(20),PM25 VARCHAR(20),PM100 VARCHAR(20),P03um_perdecilitre VARCHAR(20),P05um_perdecilitre VARCHAR(20),P10um_perdecilitre VARCHAR(20),P25um_perdecilitre VARCHAR(20),P50um_perdecilitre VARCHAR(20),P100um_perdecilitre VARCHAR(20),CO2_ppm VARCHAR(20),TVOC_ppb VARCHAR(20),AIR_QUALITY_ppm VARCHAR(20),GAS_RESISTANCE_KOhms VARCHAR(20),TIME TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP)';
    db.query(sql,(err,result)=>{
        if(err) throw err;
        console.log(result);
        res.send("Tablecreated.....");
    });
});

//welcome route
app.get('/',(req,res)=>{
    res.send('<h1>!WELCOME TO YOUR DOCKER!!</h1>')
});
//inert data
app.get('/insert',(req,res)=>{
    //res.setHeader("Acces-Control-Allow-Origin","*");
    //console.log(req.query);
    let post=req.query;
    let sql ='INSERT INTO air_quality SET ?';
    db.query(sql,post,(err,result)=>{
        if(err) throw err;
        console.log(result);
        res.send(req.query);
    });

});
const port=process.env.PORT || 3000;
app.listen(port,()=>{
    console.log(`Server started on port :${port}`);
})

