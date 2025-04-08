const Db_Connection= require('../src/database')

exports.push = (req, res) => {
    parameter_name = req.params.name;
    parameter_value = req.params.value;
    Db_Connection.Open(Cn => {
        Db_Connection.ExecuteQuery(Cn, "INSERT INTO PARAMETER_STATUS VALUES (@Name, @Value, CURRENT_TIMESTAMP)", { "Name": parameter_name, "Value": parameter_value }, (Results) => {
            res.json("Record Saved");
        });
    })
};

exports.fetch = (req, res) => {
    qry = `
        with Latest AS (
            SELECT
                PARAMETER_ID,
                MAX(TIMESTAMP) AS TS
            FROM PARAMETER_STATUS
            GROUP BY PARAMETER_ID
        )
        SELECT
            C.PARAMETER_ID AS ID,
            P.PARAMETER_NAME AS NAME,
            CONCAT('info-box text-bg-', P.COLOR) AS STYLE,
            P.ICON,
            CONCAT(C.STATUS_VALUE, CASE WHEN C.PARAMETER_ID = 4 THEN ' PPM' ELSE '' END) AS VALUE
        FROM PARAMETER_STATUS AS C
            INNER JOIN Latest AS L ON C.PARAMETER_ID = L.PARAMETER_ID AND C.TIMESTAMP = L.TS
            INNER JOIN PARAMETER AS P ON P.PARAMETER_ID = C.PARAMETER_ID
        ORDER BY
            C.PARAMETER_ID
    `
    // res.render('homepage', { Status: null });

    Db_Connection.Open(Connection => {
        Db_Connection.ExecuteQuery(Connection, qry, null, (_Cn, Results) => {
            ParamStatus = [];
            Results.recordset.forEach(result => { 
                ParamStatus.push(result);
            });
            console.log(ParamStatus);
            res.render('homepage', { Title: "Current Status", Status: ParamStatus });
        });
    });
};

exports.refresh = (req, res) => {
    qry = `
        with Latest AS (
            SELECT
                PARAMETER_ID,
                MAX(TIMESTAMP) AS TS
            FROM PARAMETER_STATUS
            GROUP BY PARAMETER_ID
        )
        SELECT
            C.PARAMETER_ID AS ID,
            P.PARAMETER_NAME AS NAME,
            CONCAT('info-box text-bg-', P.COLOR) AS STYLE,
            P.ICON,
            CONCAT(C.STATUS_VALUE, CASE WHEN C.PARAMETER_ID = 4 THEN ' PPM' ELSE '' END) AS VALUE
        FROM PARAMETER_STATUS AS C
            INNER JOIN Latest AS L ON C.PARAMETER_ID = L.PARAMETER_ID AND C.TIMESTAMP = L.TS
            INNER JOIN PARAMETER AS P ON P.PARAMETER_ID = C.PARAMETER_ID
        ORDER BY
            C.PARAMETER_ID
    `
    Db_Connection.Open(Connection => {
        Db_Connection.ExecuteQuery(Connection, qry, null, (_Cn, Results) => {
            ParamStatus = [];
            Results.recordset.forEach(result => { 
                ParamStatus.push(result);
            });
            console.log(ParamStatus);
            res.json(ParamStatus);
            // res.json(ParamStatus);
        });
    });
};

exports.nurtrients = (req, res) => {
    qry = `SELECT TIMESTAMP, CONCAT(STATUS_VALUE, ' PPM') AS VALUE FROM PARAMETER_STATUS WHERE PARAMETER_ID = 4 AND STATUS_VALUE NOT IN ('ON', 'OFF') ORDER BY TIMESTAMP DESC`;
    Db_Connection.Open(Connection => {
        Db_Connection.ExecuteQuery(Connection, qry, null, (_Cn, Results) => {
            NutrientLevels = Results.recordset;
            res.render('TableGraph', { Title: "Nutrient Level", SubTitle: "Dissolved Nutrients over time", Data: JSON.stringify(NutrientLevels), Status: NutrientLevels });
        });
    });
};

exports.water_pump = (req, res) => {
    qry = `SELECT TIMESTAMP, CONCAT(STATUS_VALUE, '') AS VALUE FROM PARAMETER_STATUS WHERE PARAMETER_ID = 1 ORDER BY TIMESTAMP DESC`;
    Db_Connection.Open(Connection => {
        Db_Connection.ExecuteQuery(Connection, qry, null, (_Cn, Results) => {
            NutrientLevels = Results.recordset;
            res.render('Table', { Title: "Water Pump", SubTitle: "Water Pump Function Status over time", Status: NutrientLevels });
        });
    });
};

exports.grow_led = (req, res) => {
    qry = `SELECT TIMESTAMP, CONCAT(STATUS_VALUE, '') AS VALUE FROM PARAMETER_STATUS WHERE PARAMETER_ID = 3 ORDER BY TIMESTAMP DESC`;
    Db_Connection.Open(Connection => {
        Db_Connection.ExecuteQuery(Connection, qry, null, (_Cn, Results) => {
            NutrientLevels = Results.recordset;
            res.render('Table', { Title: "GROW LED", SubTitle: "Grow LED Function Status over time", Status: NutrientLevels });
        });
    });
};

exports.o2_motor = (req, res) => {
    qry = `SELECT TIMESTAMP, CONCAT(STATUS_VALUE, '') AS VALUE FROM PARAMETER_STATUS WHERE PARAMETER_ID = 2 ORDER BY TIMESTAMP DESC`;
    Db_Connection.Open(Connection => {
        Db_Connection.ExecuteQuery(Connection, qry, null, (_Cn, Results) => {
            NutrientLevels = Results.recordset;
            res.render('Table', { Title: "Oxygen Motor", SubTitle: "Oxygen Motor Function Status over time", Status: NutrientLevels });
        });
    });
};