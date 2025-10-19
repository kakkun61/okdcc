import * as Logic from "@/logic";

declare const __brand: unique symbol;

type Brand<T, TBrand> = T & { [__brand]: TBrand };

export type ThrottleInfoId = Brand<string, "ThrottleInfoId">;

export const createThrottleInfoId = (id: string): ThrottleInfoId =>
  id.length > 0
    ? (id as ThrottleInfoId)
    : (() => {
        throw new Error("ThrottleInfoId must be non-empty");
      })();

export type ThrottleInfo = Logic.ThrottleInfo & { id: ThrottleInfoId };

export const createThrottleInfoKey = (id: ThrottleInfoId): string =>
  `throttle info: ${id}`;

export const throttleInfoIds = "throttle info ids";

export const readThrottleInfo = (
  storage: Storage,
  id: ThrottleInfoId,
): ThrottleInfo => {
  const value = storage.getItem(createThrottleInfoKey(id));
  if (value === null) {
    throw new Error(`FunctionInfo with id "${id}" not found in storage`);
  }
  return { ...Logic.parseThrottleInfo(JSON.parse(value)), id };
};

export const writeThrottleInfo = (
  storage: Storage,
  throttleInfo: ThrottleInfo,
): void => {
  storage.setItem(
    createThrottleInfoKey(throttleInfo.id),
    JSON.stringify(throttleInfo),
  );
};

export const readThrottleInfoIdsAll = (storage: Storage): ThrottleInfoId[] => {
  const value = storage.getItem(throttleInfoIds);
  if (value === null) {
    return [];
  }
  return JSON.parse(value).map(createThrottleInfoId);
};

export const writeThrottleInfoIdsAll = (
  storage: Storage,
  ids: ThrottleInfoId[],
): void => {
  storage.setItem(throttleInfoIds, JSON.stringify(ids));
};

export const writeThrottleInfos = (
  storage: Storage,
  throttleInfos: ThrottleInfo[],
): void => {
  for (const throttleInfo of throttleInfos) {
    writeThrottleInfo(storage, throttleInfo);
  }
  const ids = throttleInfos.map((info) => info.id);
  writeThrottleInfoIdsAll(storage, ids);
};

export const readThrottleInfosAll = (
  storage: Storage,
): ThrottleInfo[] => {
  const ids = readThrottleInfoIdsAll(storage);
  return ids.map((id) => readThrottleInfo(storage, id));
};

export const exampleThrottleInfos: ThrottleInfo[] =
  Logic.exampleThrottleInfos.map((info, index) => ({
    ...info,
    id: createThrottleInfoId(index.toString()),
  }));
