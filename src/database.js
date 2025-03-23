var azure = require('mssql')

module.exports = {
    Open: function(Callback) {
        var Connection = new azure.ConnectionPool({
            user: 'hydroponics_admin',
            password: '2004KuttY@27',
            server: 'hydroponics.database.windows.net',
            database: 'hydrophonics'
        });
        Connection.connect().then(() => {
            Callback(Connection)
        });
    },
    ExecuteQuery: function(Connection, Query, Params, Callback) {
        if (Connection != null) {
            var req = Connection.request();
            if (Params == null)
                Params = { };
            Object.keys(Params).forEach((input, _index) => {
                req.input(input, Params[input]);
            });
            req.query(Query).then(Result => {
                Callback(Connection, Result);
            });
        }
    },
    RunProcedure: function(Connection, ProcedureName, Params, Callback) {
        if (Connection != null) {
            var req = Connection.request();
            Object.keys(Params).forEach((input, _index) => {
                req.input(input, Params[input]);
            });
            req.execute(ProcedureName).then(Result => {
                Callback(Connection, Result);
            });
        }
    }
};