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

export interface FunctionInfo<Image> {
  label: string;
  number: FunctionNumber;
  image?: string | Image;
  stateType: "momentary" | "toggle";
}
