package output

import (
	"fmt"
	"time"

	"google3/third_party/upb/benchmarks/run_android_benchmarks/benchmarks"
)

type ResultValue struct {
	New   string
	Delta string
}

type ResultValues struct {
	Cpu          ResultValue
	Instructions ResultValue
	IPC          ResultValue
	CacheMisses  ResultValue
	BranchMisses ResultValue
}

type ResultLine struct {
	Invocation *Invocation
	Benchmark  *benchmarks.Benchmark
	Header     string
	Value      *string
}

type Invocation struct {
	Params             benchmarks.InvocationParams
	Reference          string
	OutputFileName     string
	BlazeOutputBaseDir string
	AndroidSerial      string
	StartTime          *time.Time // nil if not yet started
	EndTime            *time.Time // nil if not yet finished
	Output             string
	ResultLines        []*ResultLine
}

type InvocationGroup struct {
	Target      *benchmarks.Target
	Invocations []*Invocation
}

// PrintTargetBanner prints a banner for the given target.
func PrintTargetBanner(target benchmarks.Target, fasttable string) int {
	fmt.Printf("Results for: %s (%s)\n", target.Label, fasttable)
	println("────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────")
	println("CPU                   Bench    Cpu                    Instructions           IPC             Cache Misses           Branch Misses")
	println("────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────")
	return 4
}

func formatResultValueWithWidth(resultValue ResultValue, last bool, width int) string {
	contents := fmt.Sprintf("%s %s", resultValue.New, resultValue.Delta)
	if last {
		return contents
	}
	return fmt.Sprintf("%*s", -width, contents)
}

func formatResultValue(resultValue ResultValue, last bool) string {
	return formatResultValueWithWidth(resultValue, last, 23)
}

func FormatResultLine(resultValues ResultValues) string {
	return formatResultValue(resultValues.Cpu, false) +
		formatResultValue(resultValues.Instructions, false) +
		formatResultValueWithWidth(resultValues.IPC, false, 16) +
		formatResultValue(resultValues.CacheMisses, false) +
		formatResultValue(resultValues.BranchMisses, true)
}

func moveCursorUp(lines int) {
	// \x1b[<n>A  Cursor Up N lines
	fmt.Printf("\x1b[%dA", lines)
}

// clearLineRaw clears the current line from the cursor to the end of the line.
func clearLine() {
	// \x1b[K or \x1b[0K Erase to End of Line
	fmt.Print("\x1b[K")
}

func SetBenchmarkHeader(line *ResultLine) {
	line.Header = fmt.Sprintf("%-21s %-9s", line.Invocation.Params.CPU.Name, line.Benchmark.ShortName)
}

/*
func printColumn(m parsebenchy.Metrics, metricName string, benchmarkName string) {
	meas := m[metricName][benchmarkName]
	st := fmt.Sprintf("%s (%s)", meas.New, meas.Delta)
	fmt.Printf("%-23s", st)
}
*/

func PrintOutput(groups []*InvocationGroup, scrollBack int) int {
	if scrollBack > 0 {
		moveCursorUp(scrollBack)
	}
	lineCount := 0
	for _, group := range groups {
		lineCount += PrintTargetBanner(*group.Target, group.Invocations[0].Params.Fasttable.Name)
		for _, invocation := range group.Invocations {
			for _, resultLine := range invocation.ResultLines {
				clearLine()
				print(resultLine.Header)
				if resultLine.Value != nil {
					fmt.Print(*resultLine.Value)
				} else if invocation.StartTime != nil {
					fmt.Printf("Running (%s)", time.Since(*invocation.StartTime).Round(time.Second))
				}
				fmt.Printf("\n")
				lineCount++
			}
		}
		clearLine()
		lineCount++
		fmt.Printf("\n")
	}
	return lineCount
}
