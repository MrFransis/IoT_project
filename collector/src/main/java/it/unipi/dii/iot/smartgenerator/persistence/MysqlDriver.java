package it.unipi.dii.iot.smartgenerator.persistence;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Properties;
import it.unipi.dii.iot.smartgenerator.utils.Utils;

public class MysqlDriver {
    private static MysqlDriver instance;
    private Connection conn = null;
    public String mysqlHost;
    public String mysqlPort;
    public String mysqlUsername;
    public String mysqlPassword;
    public String mysqlDatabase;

    private MysqlDriver(Properties configurationParameters){
        this.mysqlHost = configurationParameters.getProperty("mysqlHost");
        this.mysqlPort =configurationParameters.getProperty("mysqlPort");
        this.mysqlUsername = configurationParameters.getProperty("mysqlUsername");
        this.mysqlPassword = configurationParameters.getProperty("mysqlPassword");
        this.mysqlDatabase = configurationParameters.getProperty("mysqlDatabase");
    };


    public static MysqlDriver getInstance() {
        if (instance == null)
            instance = new MysqlDriver(Utils.readConfigurationParameters());
        return instance;
    }

    /**
     * Method that connects to mongoDB and returns the MongoClient instance
     */
    public Connection openConnection() {
        if (conn != null)
            return conn;

        try
        {
            //Class.forName("com.mysql.jdbc.Driver");
            String url = "jdbc:mysql://"+ mysqlHost +":"+mysqlPort+"/"+mysqlDatabase;
            try {
                conn = DriverManager.getConnection(url, mysqlUsername, mysqlPassword);
            }
            catch (SQLException e) {
                e.printStackTrace();
            }
            return conn;
        }
        catch (Exception ex)
        {
            System.out.println("MySQL is not available");
            return null;
        }
    }

    /**
     * Method used to close the connection
     * @throws SQLException
     */
    public void closeConnection() throws SQLException {
        if (conn != null){
            System.out.println("Connection closed ...");
            conn.close();
        }
    }  
}
