const express=require('express');
const mysql=require('mysql2');
const cors=require('cors');

const connectWithREtry = () =>{
///Create connection 
const db=mysql.createConnection({
    host:"database",
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
}

connectWithREtry();

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
//create table
app.get('/createtable',(req,res)=>{
    let sql ='CREATE TABLE air_quality(id INT AUTO_INCREMENT PRIMARY KEY ,temperature VARCHAR(20),humidity VARCHAR(20),altitude VARCHAR(20),pressure VARCHAR(20),PM10 VARCHAR(20),PM25 VARCHAR(20),PM100 VARCHAR(20),P03um VARCHAR(20),P05um VARCHAR(20),P10um VARCHAR(20),P25um VARCHAR(20),P50um VARCHAR(20),P100um VARCHAR(20),CO2 VARCHAR(20),TVOC VARCHAR(20),AIR_QUALITY VARCHAR(20),GAS_RESISTANCE VARCHAR(20),TIME TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP)';
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
    //const data = res.end(JSON.stringify(req.query));
});
const port=process.env.PORT || 3000;
app.listen(port,()=>{
    console.log(`Server started on port :${port}`);
})