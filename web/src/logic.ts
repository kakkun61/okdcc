// logic.ts はプロジェクトの他の何にも依存しない
// React や Next.js にも依存しない

declare const __brand: unique symbol;

type Brand<T, TBrand> = T & { [__brand]: TBrand };

export type FunctionNumber = Brand<number, "FunctionNumber">;

export const createFunctionNumber = (num: number): FunctionNumber =>
  0 <= num && num <= maxFunctionNumber
    ? (num as FunctionNumber)
    : (() => {
        throw new Error(
          `FunctionNumber must be between 0 and ${maxFunctionNumber}`,
        );
      })();

export const maxFunctionNumber = 68 as FunctionNumber;

export const frontLightFunctionNumber = 0 as FunctionNumber;

export type TrainName = Brand<string, "TrainName">;

export const createTrainName = (name: string): TrainName =>
  name.length > 0
    ? (name as TrainName)
    : (() => {
        throw new Error("TrainName must be non-empty");
      })();

export type ImageId = { kind: "builtin"; id: string };

export const createBuiltinImageId = (id: string): ImageId => ({
  kind: "builtin",
  id,
});

export type FunctionInfo = {
  label: string;
  number: FunctionNumber;
  image?: ImageId;
  stateType: "momentary" | "toggle";
};

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

export type App = {
  state: AppState;
};

export type AppState = AppStateThrottle | AppStateDebug;

export type AppStateThrottle = {
  type: "AppStateThrottle";
  throttleStates: ThrottleState[];
};

export type AppStateDebug = {
  type: "AppStateDebug";
};

export type ThrottleState = {
  throttleInfo: {};
  direction: "forward" | "backward";
  speed: Speed;
};

export const initialApp: App = {
  state: {
    type: "AppStateDebug",
  },
};

export const moveToThrottleState = (
  _: App,
  throttleStates: { throttleInfo: {} }[],
): App => {
  return {
    state: {
      type: "AppStateThrottle",
      throttleStates: throttleStates.map((ts) => ({
        throttleInfo: ts.throttleInfo,
        direction: "forward",
        speed: "Stop",
      })),
    },
  };
};

export type AppAction = {
  type: "start";
  throttleStates: { throttleInfo: {} }[];
};

export const appDispatch = (app: App, action: AppAction) => {
  switch (app.state.type) {
    case "AppStateDebug":
      switch (action.type) {
        case "start":
          return moveToThrottleState(app, action.throttleStates);
      }
  }
  throw new Error(
    `Unhandled action: {app: ${JSON.stringify(app)}, action: ${JSON.stringify(action)}}`,
  );
};
