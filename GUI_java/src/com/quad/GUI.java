package com.quad;

/**
 * Created by Jocke on 2014-07-18.
 */

import org.lwjgl.LWJGLException;
import org.lwjgl.Sys;
import org.lwjgl.opengl.Display;
import org.lwjgl.opengl.DisplayMode;
import org.lwjgl.opengl.GL11;
import org.newdawn.slick.TrueTypeFont;
import org.newdawn.slick.Color;

import java.awt.*;

public class GUI {

    private final int timeBetweenUpdates = 10; //ms

    private TrueTypeFont font;
    private boolean antiAlias = true;

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
                short thrust = (short)Math.max(0, controllerManager.getAxisThrustValue() * -1000.0f);
                if (controllerManager.isButtonTrianglePressed()) {
                    thrust = 150;
                }
                else if (controllerManager.isButtonCirclePressed()) {
                    thrust = 300;
                }
                else if (controllerManager.isButtonCrossPressed()) {
                    thrust = 450;
                }
                else if (controllerManager.isButtonSquarePressed()) {
                    thrust = 600;
                }
                serialEventManager.setTxFieldOfType(SerialEventManager.DataTxEnum.DataTxControlThrust, thrust);

                //TODO: rätt håll? -500?
                short yaw = (short) (controllerManager.getAxisYawValue() * 500.0f);
                serialEventManager.setTxFieldOfType(SerialEventManager.DataTxEnum.DataTxControlYaw, yaw);

                short pitch = (short) (controllerManager.getAxisPitchValue() * 500.0f);
                serialEventManager.setTxFieldOfType(SerialEventManager.DataTxEnum.DataTxControlPitch, pitch);

                short roll = (short) (controllerManager.getAxisRollValue() * 500.0f);
                serialEventManager.setTxFieldOfType(SerialEventManager.DataTxEnum.DataTxControlRoll, roll);

                serialEventManager.writeSerial();

                //System.out.println(serialEventManager.getRxFieldOfType(SerialEventManager.DataRxEnum.DataRxAngleX) / 91.0f);

                //TODO: draw hud
                renderGUI();


            }

            // render OpenGL here

            serialEventManager.readSerial();


        }

        serialEventManager.stop();

        Display.destroy();
        System.out.println("Stopping GUI");

    }

    private void setupGUI() {
        initGL(800, 600);

        Font awtFont = new Font("Terminal", Font.PLAIN, 15);
        font = new TrueTypeFont(awtFont, antiAlias);

        lastTimeStamp = getTime();
        // init OpenGL here
    }

    private void initGL(int width, int height) {
        try {
            Display.setDisplayMode(new DisplayMode(width,height));
            Display.create();
            Display.setVSyncEnabled(true);
        } catch (LWJGLException e) {
            e.printStackTrace();
            System.exit(0);
        }

        GL11.glEnable(GL11.GL_TEXTURE_2D);
        GL11.glShadeModel(GL11.GL_SMOOTH);
        GL11.glDisable(GL11.GL_DEPTH_TEST);
        GL11.glDisable(GL11.GL_LIGHTING);

        GL11.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        GL11.glClearDepth(1);

        GL11.glEnable(GL11.GL_BLEND);
        GL11.glBlendFunc(GL11.GL_SRC_ALPHA, GL11.GL_ONE_MINUS_SRC_ALPHA);

        GL11.glViewport(0,0,width,height);
        GL11.glMatrixMode(GL11.GL_MODELVIEW);

        GL11.glMatrixMode(GL11.GL_PROJECTION);
        GL11.glLoadIdentity();
        GL11.glOrtho(0, width, height, 0, 1, -1);
        GL11.glMatrixMode(GL11.GL_MODELVIEW);
    }

    private void renderGUI() {
        GL11.glClear(GL11.GL_COLOR_BUFFER_BIT);

        //Color.white.bind();
        font.drawString(10, 10, "Batt: " + serialEventManager.getRxFieldOfType(SerialEventManager.DataRxEnum.DataRxBatteryLevel), Color.green);
        font.drawString(10, 30, "CPU: " + serialEventManager.getRxFieldOfType(SerialEventManager.DataRxEnum.DataRxDutyTime), Color.green);
        font.drawString(10, 50, "Valid: " + serialEventManager.getNumValidPackets(), Color.green);
        font.drawString(10, 70, "Failed: " + serialEventManager.getNumFailedPackets(), Color.green);
        font.drawString(10, 90, "Rec Valid: " + serialEventManager.getRxFieldOfType(SerialEventManager.DataRxEnum.DataRxNumValidPackets), Color.green);
        font.drawString(10, 110, "Rec Failed: " + serialEventManager.getRxFieldOfType(SerialEventManager.DataRxEnum.DataRxNumFailedPackets), Color.green);
        font.drawString(10, 130, "Pitch: " + (serialEventManager.getRxFieldOfType(SerialEventManager.DataRxEnum.DataRxAngleY) / 91.0f), Color.green);
        font.drawString(10, 150, "Roll: " + (serialEventManager.getRxFieldOfType(SerialEventManager.DataRxEnum.DataRxAngleX) / 91.0f), Color.green);

        font.drawString(10, 200, "Thrust: " + (serialEventManager.getTxFieldOfType(SerialEventManager.DataTxEnum.DataTxControlThrust)), Color.green);


        Display.update();
        //Display.sync(100);
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