"use client";

import { useEffect, useState } from "react";

import * as Io from "@/io";
import * as Logic from "@/logic";
import { Page } from "@/components/Page";

export default () => {
  const [throttleInfoIds, setThrottleInfoIds] = useState<
    Io.ThrottleInfoId[] | undefined
  >(undefined);
  useEffect(() => {
    setThrottleInfoIds(Io.readThrottleInfoIdsAll(localStorage));
  }, []);
  return (
    <Page>
      <h1>Debug Mode</h1>
      {(() => {
        if (typeof throttleInfoIds === "undefined") {
          return <p>Loading...</p>;
        }
        if (throttleInfoIds.length === 0) {
          return <p>None</p>;
        }
        return (
          <form method="get" action="/throttle">
            <fieldset>
              {throttleInfoIds.map((id) => (
                <div key={id}>
                  <input
                    type="checkbox"
                    id={id}
                    name={Logic.throttlePageIdParamKey}
                    value={id}
                  />
                  <label htmlFor={id}>{id}</label>
                </div>
              ))}
              <input type="submit" />
            </fieldset>
          </form>
        );
      })()}
      <p>
        <button
          onClick={() => {
            Io.writeThrottleInfos(localStorage, Io.exampleThrottleInfos);
            setThrottleInfoIds(Io.exampleThrottleInfos.map((info) => info.id));
          }}
        >
          Save Example to Localstorage
        </button>
      </p>
    </Page>
  );
};
