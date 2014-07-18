package com.quad;

/**
 * Created by Jocke on 2014-07-18.
 */

import org.lwjgl.LWJGLException;
import org.lwjgl.Sys;
import org.lwjgl.opengl.Display;
import org.lwjgl.opengl.DisplayMode;

public class GUI {

    private final int timeBetweenUpdates = 10; //ms

    private SerialEventManager serialEventManager;

    private ControllerManager controllerManager;

    private long lastTimeStamp;

    public GUI() {
        serialEventManager = new SerialEventManager("COM6", 38400);
        controllerManager = new ControllerManager("Trust Gamepad");
    }

    public void startGUI() {
        System.out.println("Starting GUI");

        setupGUI();

        serialEventManager.initSerialEventManager();
        controllerManager.initControllerManager();


        while (!Display.isCloseRequested()) {

            //somewhat reasonable timing resolution
            if (getTime() - lastTimeStamp >= timeBetweenUpdates) {
                lastTimeStamp = getTime();

                //poll new values from controller
                controllerManager.pollController();

                //transfer values from controller to serial manager
                short thrust = (short)Math.max(0, controllerManager.getAxisThrustValue() * 1000.0f);
                serialEventManager.setTxFieldOfType(SerialEventManager.DataTxEnum.DataTxControlThrust, thrust);

                short yaw = (short) (controllerManager.getAxisYawValue() * 500.0f);
                serialEventManager.setTxFieldOfType(SerialEventManager.DataTxEnum.DataTxControlYaw, yaw);

                short pitch = (short) (controllerManager.getAxisPitchValue() * 500.0f);
                serialEventManager.setTxFieldOfType(SerialEventManager.DataTxEnum.DataTxControlPitch, pitch);

                short roll = (short) (controllerManager.getAxisRollValue() * 500.0f);
                serialEventManager.setTxFieldOfType(SerialEventManager.DataTxEnum.DataTxControlRoll, roll);

                serialEventManager.writeSerial();

                //System.out.println(serialEventManager.getRxFieldOfType(SerialEventManager.DataRxEnum.DataRxAngleX) / 91.0f);

                //TODO: draw hud

                Display.update();
            }

            // render OpenGL here

            serialEventManager.readSerial();


        }

        serialEventManager.stop();

        Display.destroy();
        System.out.println("Stopping GUI");

    }

    private void setupGUI() {
        try {
            Display.setDisplayMode(new DisplayMode(800, 600));
            Display.create();
        } catch (LWJGLException e) {
            e.printStackTrace();
        }

        lastTimeStamp = getTime();
        // init OpenGL here
    }

    private void renderGUI() {

    }

    /**
     * Get the accurate system time
     *
     * @return The system time in milliseconds
     */
    private long getTime() {
        return (Sys.getTime() * 1000) / Sys.getTimerResolution();
    }

}