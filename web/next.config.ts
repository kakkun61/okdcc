import type { NextConfig } from "next";

const nextConfig: NextConfig = {
  images: {
    localPatterns: [
      {
        pathname: "src/assets/**",
      },
    ],
  },
};

export default nextConfig;
