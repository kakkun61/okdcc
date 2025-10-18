"use client";

import { ThrottleProps } from "@/components/Throttle";
import * as Logic from "@/logic";
import lightImage from "@/assets/light.png";
import { ThrottleList } from "@/components/ThrottleList";
import { useState } from "react";
import { AppContext, useAppContext } from "@/components/AppContext";

const throttleProps: ThrottleProps = {
  trainName: Logic.createTrainName("DD-51"),
  functionInfos: [
    {
      number: Logic.createFunctionNumber(1),
      label: "Light",
      image: lightImage,
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(2),
      label: "Horn",
      stateType: "momentary",
    },
    {
      number: Logic.createFunctionNumber(3),
      label: "Bell",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(4),
      label: "Squeal",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(5),
      label: "Coupler",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(6),
      label: "Mute",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(7),
      label: "Smoke",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(8),
      label: "Generator",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(9),
      label:
        "Extremely Too Long Long Long Long Long Long Long Long Long Long Name",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(10),
      label: "Placeholder 1",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(11),
      label: "Placeholder 2",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(12),
      label: "Placeholder 3",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(13),
      label: "Placeholder 4",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(14),
      label: "Placeholder 5",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(15),
      label: "Placeholder 6",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(16),
      label: "Placeholder 7",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(17),
      label: "Placeholder 8",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(18),
      label: "Placeholder 9",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(19),
      label: "Placeholder 10",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(20),
      label: "Placeholder 11",
      stateType: "toggle",
    },
    {
      number: Logic.createFunctionNumber(21),
      label: "Placeholder 12",
      stateType: "toggle",
    },
  ],
};

const throttlePropsList: ThrottleProps[] = [
  throttleProps,
  { ...throttleProps, trainName: Logic.createTrainName("C57") },
  { ...throttleProps, trainName: Logic.createTrainName("JRW 221") },
];

const Home = () => {
  const { app, appDispatch } = useAppContext();
  switch (app.state.type) {
    case "AppStateDebug":
      return (
        <>
          <h1>Debug Mode</h1>
          <button
            onClick={() =>
              appDispatch({
                type: "start",
                throttleStates: [{ throttleInfo: {} }],
              })
            }
          >
            Start
          </button>
        </>
      );
    case "AppStateThrottle":
      return <ThrottleList throttlePropsList={throttlePropsList} />;
  }
};

export default Home;
