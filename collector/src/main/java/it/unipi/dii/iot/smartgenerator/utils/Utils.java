package it.unipi.dii.iot.smartgenerator.utils;

import java.io.FileInputStream;
import java.util.Properties;

public class Utils {
    public static Properties readConfigurationParameters(){
        try{
            FileInputStream fis = new FileInputStream("./src/config.properties");
            //FileInputStream fis = new FileInputStream("collector/src/config.properties");
            Properties prop = new Properties();
            prop.load(fis);
            return prop;
        }
        catch(Exception e){
            e.printStackTrace();
        }
        return null;
    }
}
