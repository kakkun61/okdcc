import { FunctionButton } from "@/components/FunctionButton";
import { Throttle } from "@/components/Throttle";

export default function Home() {
  return (
    <Throttle>
      <FunctionButton number={1} />
    </Throttle>
  );
}
