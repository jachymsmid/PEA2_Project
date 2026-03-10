import matplotlib.pyplot as plt
import numpy as np
import sys
import re
import os

# parse the standardized output file
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

    # what file to process, for example n8192, n16384, ...
    filename = sys.argv[1]
    # specific solver used, for example jacobi_verision_2
    solver_name = sys.argv[2]

    # hardware specific data, mby need correcting
    PEAK_BW_GBPS = 37.659
    PEAK_GFLOPS = 998.4
    APP_AI = 0.75

    # parse the output file
    APP_GFLOPS, APP_BW = parse_benchmark(filename)

    # x axis, log scale
    ai_vals = np.logspace(-2, 2, 500)

    roofline_vals = np.minimum(PEAK_GFLOPS, PEAK_BW_GBPS * ai_vals)

    # plot the roofs
    plt.figure(figsize=(10, 6))
    plt.plot(ai_vals, roofline_vals, color='black', linewidth=2)

    # performance of the code
    plt.scatter(APP_AI, APP_GFLOPS, color='red', s=20, zorder=5, label=f'{solver_name} ({APP_GFLOPS} GFLOPS)')

    # theoretical best performance
    plt.axvline(x=APP_AI, color='gray', linestyle='--', alpha=0.7)

    # labels and titles
    plt.xscale('log')
    plt.yscale('log')
    plt.xlabel('Arithmetic Intensity (FLOPs / Byte)', fontsize=12)
    plt.ylabel('Performance (GFLOPS)', fontsize=12)
    plt.title(f'Roofline Model for {filename}', fontsize=14)

    # grid
    plt.grid(True, which="both", ls="--", alpha=0.5)
    plt.legend()
    plt.tight_layout()

    plt.savefig(f"roofline_{filename}_{solver_name}.png")

if __name__ == "__main__":
    main()
