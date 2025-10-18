"use client";

import React from "react";
import * as Logic from "@/logic";

const Context = React.createContext<
  { app: Logic.App; appDispatch: React.Dispatch<Logic.AppAction> } | undefined
>(undefined);

export const AppContext = ({ children }: { children: React.ReactNode }) => {
  const [app, appDispatch] = React.useReducer(
    Logic.appDispatch,
    Logic.initialApp,
  );
  return (
    <Context.Provider value={{ app, appDispatch }}>{children}</Context.Provider>
  );
};

export const useAppContext = () => {
  const context = React.useContext(Context);
  if (!context) {
    throw new Error("useApp must be used within an AppContext");
  }
  return context;
};
