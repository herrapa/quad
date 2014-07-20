package com.quad;

import org.lwjgl.LWJGLException;
import org.lwjgl.input.Controller;
import org.lwjgl.input.Controllers;

/**
 * Created by Jocke on 2014-07-18.
 */
public class ControllerManager {

    static Controller controller = null;
    private String controllerName;

    private boolean buttonTrianglePressed; // 0
    private boolean buttonCirclePressed; // 1
    private boolean buttonCrossPressed; // 2
    private boolean buttonSquarePressed; // 3

    private float axisThrustValue = 0.0f;
    private float axisYawValue = 0.0f;
    private float axisPitchValue = 0.0f;
    private float axisRollValue = 0.0f;

    private boolean axisThrustIsInitialized = false;
    private boolean axisYawIsInitialized = false;
    private boolean axisPitchIsInitialized = false;
    private boolean axisRollIsInitialized = false;


    public ControllerManager(String name) {

        try {
            Controllers.create();
        } catch (LWJGLException e) {
            e.printStackTrace();
        }
        controllerName = name;
    }

    public void initControllerManager() {

        Controllers.poll();

        for (int i = 0; i < Controllers.getControllerCount();i++) {
            Controller c = Controllers.getController(i);
            System.out.println(c.getName());
            if (c.getName().equals(controllerName)) {
                controller = c;
                break;
            }
        }

        for (int i = 0; i < controller.getAxisCount(); i++) {
            System.out.println(controller.getAxisName(i));
            //controller.setDeadZone(i, 0.01f);
            //TODO: set dead zone!? or not, whatevs
        }

    }

    public void pollController() {
        controller.poll();
        buttonTrianglePressed = controller.isButtonPressed(0);
        buttonCirclePressed = controller.isButtonPressed(1);
        buttonCrossPressed = controller.isButtonPressed(2);
        buttonSquarePressed = controller.isButtonPressed(3);

        //workaround for all axis returns -1 if not moved

        //Thrust
        if (axisThrustIsInitialized){
            axisThrustValue = controller.getAxisValue(1);
        }
        else if (!axisThrustIsInitialized && (controller.getAxisValue(1) != -1.0f)) {
            axisThrustIsInitialized = true;
            axisThrustValue = controller.getAxisValue(1);
        }

        //Yaw
        if (axisYawIsInitialized){
            axisYawValue = controller.getAxisValue(3);
        }
        else if (!axisYawIsInitialized && (controller.getAxisValue(3) != -1.0f)) {
            axisYawIsInitialized = true;
            axisYawValue = controller.getAxisValue(3);
        }

        //Pitch
        if (axisPitchIsInitialized){
            axisPitchValue = controller.getAxisValue(0);
        }
        else if (!axisPitchIsInitialized && (controller.getAxisValue(0) != -1.0f)) {
            axisPitchIsInitialized = true;
            axisPitchValue = controller.getAxisValue(0);
        }

        //Roll
        if (axisRollIsInitialized){
            axisThrustValue = controller.getAxisValue(2);
        }
        else if (!axisRollIsInitialized && (controller.getAxisValue(2) != -1.0f)) {
            axisRollIsInitialized = true;
            axisRollValue = controller.getAxisValue(2);
        }
    }

    public boolean isButtonSquarePressed() {
        return buttonSquarePressed;
    }

    public boolean isButtonTrianglePressed() {
        return buttonTrianglePressed;
    }

    public boolean isButtonCirclePressed() {
        return buttonCirclePressed;
    }

    public boolean isButtonCrossPressed() {
        return buttonCrossPressed;
    }

    public float getAxisThrustValue() {
        return axisThrustValue;
    }

    public float getAxisYawValue() {
        return axisYawValue;
    }

    public float getAxisPitchValue() {
        return axisPitchValue;
    }

    public float getAxisRollValue() {
        return axisRollValue;
    }

}
