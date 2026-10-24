$ErrorActionPreference = 'Stop'

$bin = Join-Path $PWD 'build'
$corpus = Join-Path $PWD 'data\corpus_50k.txt'
$results = Join-Path $PWD 'results'

foreach ($tool in @('build_dynamic.exe', 'verify.exe', 'gen_corpus.exe')) {
    if (-not (Test-Path -LiteralPath (Join-Path $bin $tool))) {
        throw "Missing executable: $tool"
    }
}

New-Item -ItemType Directory -Force -Path $results | Out-Null

if (-not (Test-Path -LiteralPath $corpus)) {
    & "$bin\gen_corpus.exe" $corpus 50000 50000 150 42
    if ($LASTEXITCODE -ne 0) {
        throw 'Corpus generation failed.'
    }
}

# Separate working directory for this execution; no automatic deletion.
$runRoot = Join-Path $PWD (
    'experiment-runs\' + [guid]::NewGuid().ToString('N')
)
New-Item -ItemType Directory -Force -Path $runRoot | Out-Null

$summary = Join-Path $results 'dynamic.csv'
'mode,n,flushes,merges,max_disk_indexes,postings_written,postings_written_with_finalize,write_amplification,seconds' |
Set-Content -LiteralPath $summary -Encoding ASCII

function Get-Metric {
    param(
        [string[]]$Lines,
        [string]$Name
    )
    $pattern = '^' + [regex]::Escape($Name) + '\s+(\S+)\s*$'
    foreach ($line in $Lines) {
        if ($line -match $pattern) {
            return $Matches[1]
        }
    }
    throw "Missing output metric: $Name"
}

foreach ($mode in @('aux', 'log')) {
    foreach ($n in @(50000, 100000, 200000, 400000)) {
        $label = "${mode}_${n}"
        $caseDir = Join-Path $runRoot $label
        $work = Join-Path $caseDir 'work'
        $outBase = Join-Path $caseDir 'final'
        $trace = Join-Path $results "trace_${label}.csv"
        $buildLog = Join-Path $results "build_${label}.txt"
        $verifyLog = Join-Path $results "verify_${label}.txt"

        New-Item -ItemType Directory -Force -Path $caseDir | Out-Null
        Write-Host "Running $label"

        $lines = @(
            & "$bin\build_dynamic.exe" `
                $mode $corpus $work $n $outBase --trace $trace
        )
        $buildExit = $LASTEXITCODE
        $lines | Set-Content -LiteralPath $buildLog -Encoding ASCII
        if ($buildExit -ne 0) {
            throw "Indexing failed: $label"
        }

        $verification = @(& "$bin\verify.exe" $corpus $outBase)
        $verifyExit = $LASTEXITCODE
        $verification |
        Set-Content -LiteralPath $verifyLog -Encoding ASCII
        $verification | ForEach-Object { Write-Host $_ }
        if ($verifyExit -ne 0) {
            throw "Verification failed: $label"
        }

        $written = $null
        $writtenFinal = $null
        foreach ($line in $lines) {
            if ($line -match '^postings_written\s+(\d+)\s+\(incl\. finalize\s+(\d+)\)$') {
                $written = $Matches[1]
                $writtenFinal = $Matches[2]
                break
            }
        }
        if ($null -eq $written) {
            throw "Cannot parse postings_written: $label"
        }

        $row = @(
            $mode
            $n
            (Get-Metric $lines 'flushes')
            (Get-Metric $lines 'merges (before final)')
            (Get-Metric $lines 'max_disk_indexes')
            $written
            $writtenFinal
            (Get-Metric $lines 'write_amplification')
            (Get-Metric $lines 'seconds')
        ) -join ','

        Add-Content -LiteralPath $summary -Value $row -Encoding ASCII
    }
}

Write-Host "Final indexes retained in: $runRoot"
Import-Csv -LiteralPath $summary | Format-Table -AutoSize