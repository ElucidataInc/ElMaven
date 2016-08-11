import plots
import run_peakdetector
import compare_output

list_of_builds = [769, 776]
run_peakdetector.run_peakdetector.run(list_of_builds)
compare_output.compare_output.compare(list_of_builds)
plots.plots.plot_comparison(list_of_builds)