import { FunctionButton } from "@/components/Throttle/FunctionButton";
import { Throttle } from "@/components/Throttle";

export default function Home() {
  return (
    <Throttle>
      <FunctionButton number={1} />
    </Throttle>
  );
}
