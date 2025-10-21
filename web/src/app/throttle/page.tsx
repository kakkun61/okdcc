"use client";

import { useEffect, useState } from "react";
import { useSearchParams } from "next/navigation";

import { ThrottleList } from "@/components/ThrottleList";
import * as Io from "@/io";
import * as Ui from "@/components/lib";
import { ThrottleProps } from "@/components/Throttle";
import * as Logic from "@/logic";
import { Page } from "@/components/Page";

export default () => {
  const [throttleInfos, setThrottleInfos] = useState<
    Io.ThrottleInfo[] | undefined
  >(undefined);
  const searchParams = useSearchParams();

  useEffect(() => {
    const throttleInfoIds = searchParams
      .getAll(Logic.throttlePageIdParamKey)
      .map(Io.createThrottleInfoId);
    setThrottleInfos(
      throttleInfoIds.map((id) => Io.readThrottleInfo(localStorage, id)),
    );
  }, [searchParams]);
  if (typeof throttleInfos === "undefined") {
    return (
      <Page>
        <p>Loading...</p>
      </Page>
    );
  }
  if (throttleInfos.length === 0) {
    return (
      <Page>
        <p>No Throttle Infos</p>
      </Page>
    );
  }
  const throttlePropsList: ThrottleProps[] = throttleInfos.map((info) => ({
    trainName: info.trainName,
    functionInfos: info.functionInfos.map(Ui.makeFunctionInfo),
  }));
  return (
    <Page>
      <ThrottleList throttlePropsList={throttlePropsList} />
    </Page>
  );
};
