// logic.ts はプロジェクトの他の何にも依存しない
// React や Next.js にも依存しない

declare const __brand: unique symbol;

type Brand<T, TBrand> = T & { [__brand]: TBrand };

export type FunctionNumber = Brand<number, "FunctionNumber">;

export const createFunctionNumber = (number: number): FunctionNumber =>
  0 <= number && number <= maxFunctionNumber
    ? (number as FunctionNumber)
    : (() => {
        throw new Error(
          `FunctionNumber must be between 0 and ${maxFunctionNumber}`,
        );
      })();

export const parseFunctionNumber = (value: any): FunctionNumber => {
  return createFunctionNumber(value);
};

export const maxFunctionNumber = 68 as FunctionNumber;

export const frontLightFunctionNumber = 0 as FunctionNumber;

export type TrainName = Brand<string, "TrainName">;

export const createTrainName = (name: string): TrainName =>
  name.length > 0
    ? (name as TrainName)
    : (() => {
        throw new Error("TrainName must be non-empty");
      })();

export const parseTrainName = (value: any): TrainName => {
  return createTrainName(value);
};

export type ImageId = { type: "builtin"; id: string };

export const parseImageId = (value: any): ImageId => ({
  type: value["type"],
  id: value["id"],
});

export type FunctionInfo = {
  label: string;
  number: FunctionNumber;
  image?: ImageId;
  stateType: "momentary" | "toggle";
};

export const parseFunctionInfo = (value: any): FunctionInfo => ({
  label: value["label"],
  number: parseFunctionNumber(value["number"]),
  image:
    typeof value["image"] === "undefined"
      ? undefined
      : parseImageId(value["image"]),
  stateType: value["stateType"],
});

export type Speed = Speed28 | Speed128;

export type Speed28 = "Stop" | "E-Stop" | Speed28_;

export type Speed28_ = Brand<number, "Speed28_">;

export const createSpeed28 = (speed: "Stop" | "E-Stop" | number): Speed28 => {
  if (speed === "Stop" || speed === "E-Stop") {
    return speed;
  }
  if (1 <= speed && speed <= 28) {
    return speed as Speed28_;
  }
  throw new Error("Speed28 must be 'Stop', 'E-Stop', or between 1 and 28");
};

export type Speed128 = "Stop" | "E-Stop" | Speed128_;

export type Speed128_ = Brand<number, "Speed128_">;

export const createSpeed128 = (speed: "Stop" | "E-Stop" | number): Speed128 => {
  if (speed === "Stop" || speed === "E-Stop") {
    return speed;
  }
  if (1 <= speed && speed <= 128) {
    return speed as Speed128_;
  }
  throw new Error("Speed128 must be 'Stop', 'E-Stop', or between 1 and 128");
};

export type ThrottleInfo = {
  trainName: TrainName;
  functionInfos: FunctionInfo[];
};

export type ThrottleState = {
  throttleInfo: ThrottleInfo;
  direction: "forward" | "backward";
  speed: Speed;
};

export const parseThrottleInfo = (value: any): ThrottleInfo => ({
  trainName: parseTrainName(value["trainName"]),
  functionInfos: value["functionInfos"].map(parseFunctionInfo),
});

export const throttlePageIdParamKey = "id";

export const exampleThrottleInfo: ThrottleInfo = {
  trainName: createTrainName("DD-51"),
  functionInfos: [
    {
      number: createFunctionNumber(1),
      label: "Light",
      image: parseImageId({ type: "builtin", id: "light" }),
      stateType: "toggle",
    },
    {
      number: createFunctionNumber(2),
      label: "Horn",
      stateType: "momentary",
    },
    {
      number: createFunctionNumber(3),
      label: "Bell",
      stateType: "toggle",
    },
    {
      number: createFunctionNumber(4),
      label: "Squeal",
      stateType: "toggle",
    },
    {
      number: createFunctionNumber(5),
      label: "Coupler",
      stateType: "toggle",
    },
    {
      number: createFunctionNumber(6),
      label: "Mute",
      stateType: "toggle",
    },
    {
      number: createFunctionNumber(7),
      label: "Smoke",
      stateType: "toggle",
    },
    {
      number: createFunctionNumber(8),
      label: "Generator",
      stateType: "toggle",
    },
  ],
};

export const exampleThrottleInfos: ThrottleInfo[] = [
  exampleThrottleInfo,
  { ...exampleThrottleInfo, trainName: createTrainName("C57") },
  { ...exampleThrottleInfo, trainName: createTrainName("JRW 221") },
];
