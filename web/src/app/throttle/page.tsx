"use client";

import { ThrottleList } from "@/components/ThrottleList";
import { useEffect, useState } from "react";
import * as Io from "@/io";
import * as Ui from "@/components/lib";
import { ThrottleProps } from "@/components/Throttle";

export default () => {
  const [throttleInfos, setThrottleInfos] = useState<Io.ThrottleInfo[]>(
    [],
  );
  useEffect(() => {
    setThrottleInfos(Io.readThrottleInfosAll(localStorage));
  }, []);
  if (throttleInfos.length === 0) {
    return <p>No Throttle Infos</p>;
  }
  const throttlePropsList: ThrottleProps[] = throttleInfos.map((info) => ({
    trainName: info.trainName,
    functionInfos: info.functionInfos.map(Ui.makeFunctionInfo)
  }));
  return <ThrottleList throttlePropsList={throttlePropsList} />;
};
