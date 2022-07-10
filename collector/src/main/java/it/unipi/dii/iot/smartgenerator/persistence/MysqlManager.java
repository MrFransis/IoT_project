package it.unipi.dii.iot.smartgenerator.persistence;

import java.sql.Connection;
import java.sql.Date;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.text.SimpleDateFormat;
import java.util.Calendar;

import it.unipi.dii.iot.smartgenerator.utils.Message;

public class MysqlManager {
    Connection conn;
    
    public MysqlManager(Connection conn) {this.conn = conn;}

    public boolean insertSample(Message msg) {
        System.out.println("Saving data record ...");
  
        String timestamp = new SimpleDateFormat("yyyy-MM-dd hh:mm:ss").format(Calendar.getInstance().getTime());

        System.out.println(msg.getTopic() + " " + msg.getMachineId() + " " + msg.getSample() + " " + msg.getUnit());

        String query = "INSERT INTO "+ msg.getTopic() + " (sample, unit, machineid) "
        + " VALUES ('"+msg.getSample()+"', '"+msg.getUnit()+"','"+msg.getMachineId()+"');";
        try{
            PreparedStatement ps = conn.prepareStatement(query);
			ps.executeUpdate();
        }catch(SQLException e){
            e.printStackTrace();
            return false;
        }
        return true;
    }


}


