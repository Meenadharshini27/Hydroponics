const express = require('express');
const app = express();
const exphbs = require('express-handlebars');
const bodyparser = require('body-parser');
const mysql = require('mysql');
const port = 3000
require('dotenv').config();

app.use(bodyparser.json());
app.use(bodyparser.urlencoded({extended:false}));
app.use(express.static("public"));

const handlebars = exphbs.create({extname:'.hbs'});
app.engine('hbs', handlebars.engine);
app.set('view engine', 'hbs');

app.use('/', require('./controller/hydroponicsContoller'));

app.listen(port, ()=>{
    console.log("Server listening on port " + port.toString());
});
