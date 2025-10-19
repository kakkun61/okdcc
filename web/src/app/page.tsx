"use client";

import Link from "next/link";
import { useEffect, useState } from "react";

import * as Io from "@/io";

export default () => {
  const [throttleInfoIds, setThrottleInfoIds] = useState<Io.ThrottleInfoId[]>(
    [],
  );
  useEffect(() => {
    setThrottleInfoIds(Io.readThrottleInfoIdsAll(localStorage));
  }, []);
  return (
    <>
      <h1>Debug Mode</h1>
      <p>
        <Link href="/throttle">Start</Link>
      </p>
      {throttleInfoIds.length === 0 ? (
        <p>None</p>
      ) : (
        <ul>
          {throttleInfoIds.map((id) => (
            <li key={id}>{id}</li>
          ))}
        </ul>
      )}
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
    </>
  );
};
