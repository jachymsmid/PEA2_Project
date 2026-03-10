import matplotlib.pyplot as plt
import numpy as np
import sys
import re
import os

def parse_benchmark(filepath):
    gflops = None
    bandwidth = None
    
    try:
        with open(filepath, 'r') as file:
            for line in file:
                match_perf = re.search(r"Performance:\s+([0-9\.]+)\s+GFLOPS", line)
                if match_perf:
                    gflops = float(match_perf.group(1))
                    
                match_bw = re.search(r"Memory Bandwidth:\s+([0-9\.]+)\s+GB/s", line)
                if match_bw:
                    bandwidth = float(match_bw.group(1))
                    
    except FileNotFoundError:
        print(f"Error: Could not find the file '{filepath}'.")
        sys.exit(1)
        
    return gflops, bandwidth

def main():

    filename = sys.argv[1]
    solver_name = sys.argv[2]

    PEAK_BW_GBPS = 51.2
    PEAK_GFLOPS = 998.4
    APP_AI = 0.75

    APP_GFLOPS, APP_BW = parse_benchmark(filename)


    ai_vals = np.logspace(-2, 2, 500)

    roofline_vals = np.minimum(PEAK_GFLOPS, PEAK_BW_GBPS * ai_vals)

    plt.figure(figsize=(10, 6))
    plt.plot(ai_vals, roofline_vals, color='black', linewidth=2)

    plt.scatter(APP_AI, APP_GFLOPS, color='red', s=20, zorder=5, label=f'{solver_name} ({APP_GFLOPS} GFLOPS)')

    plt.axvline(x=APP_AI, color='gray', linestyle='--', alpha=0.7)

    plt.xscale('log')
    plt.yscale('log')
    plt.xlabel('Arithmetic Intensity (FLOPs / Byte)', fontsize=12)
    plt.ylabel('Performance (GFLOPS)', fontsize=12)
    plt.title(f'Roofline Model for {filename}', fontsize=14)

    plt.grid(True, which="both", ls="--", alpha=0.5)
    plt.legend()
    plt.tight_layout()

    plt.show()

if __name__ == "__main__":
    main()
