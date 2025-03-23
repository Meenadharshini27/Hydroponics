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
    Db_Connection.Open(Connection => {
        Db_Connection.ExecuteQuery(Connection, qry, null, (_Cn, Results) => {
            ParamStatus = [];
            Results.recordset.forEach(result => { 
                ParamStatus.push(result);
            });
            console.log(ParamStatus);
            res.render('homepage', { Status: ParamStatus });
            // res.json(ParamStatus);
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