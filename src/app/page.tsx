import { ThrottleProps } from "@/components/Throttle";
import * as Dcc from "@/lib/dcc";
import lightImage from "@/assets/light.png";
import { ThrottleList } from "@/components/ThrottleList";

const throttleProps: ThrottleProps = {
  trainName: Dcc.createTrainName("DD-51"),
  functionInfos: [
    {
      number: Dcc.createFunctionNumber(1),
      label: "Light",
      image: lightImage,
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(2),
      label: "Horn",
      stateType: "momentary",
    },
    {
      number: Dcc.createFunctionNumber(3),
      label: "Bell",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(4),
      label: "Squeal",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(5),
      label: "Coupler",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(6),
      label: "Mute",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(7),
      label: "Smoke",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(8),
      label: "Generator",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(9),
      label:
        "Extremely Too Long Long Long Long Long Long Long Long Long Long Name",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(10),
      label: "Placeholder 1",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(11),
      label: "Placeholder 2",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(12),
      label: "Placeholder 3",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(13),
      label: "Placeholder 4",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(14),
      label: "Placeholder 5",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(15),
      label: "Placeholder 6",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(16),
      label: "Placeholder 7",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(17),
      label: "Placeholder 8",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(18),
      label: "Placeholder 9",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(19),
      label: "Placeholder 10",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(20),
      label: "Placeholder 11",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(21),
      label: "Placeholder 12",
      stateType: "toggle",
    },
  ],
};

const throttlePropsList: ThrottleProps[] = [
  throttleProps,
  { ...throttleProps, trainName: Dcc.createTrainName("C57") },
  { ...throttleProps, trainName: Dcc.createTrainName("JRW 221") },
];

const Home = () => {
  return <ThrottleList throttlePropsList={throttlePropsList} />;
};

export default Home;
