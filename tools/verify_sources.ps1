# Re-check the verbatim excerpts against the original published text.
# Requires internet access; the quiz itself is completely offline.
$ErrorActionPreference = 'Stop'
$quizRoot = Split-Path $PSScriptRoot -Parent
$quizNotes = Get-Content (Join-Path $quizRoot 'quote-sources.json') -Raw -Encoding UTF8 | ConvertFrom-Json
$quizHeader = Get-Content (Join-Path $quizRoot 'src/literary_notes.h') -Raw -Encoding UTF8
$quizCache = @{}
foreach ($quizNote in $quizNotes) {
    if (-not $quizCache.ContainsKey($quizNote.url)) {
        $quizResponse = Invoke-WebRequest -Uri $quizNote.url -UseBasicParsing -TimeoutSec 30
        $quizCache[$quizNote.url] = [System.Net.WebUtility]::HtmlDecode(($quizResponse.Content -replace '<[^>]+>', ' ' -replace '\s+', ' '))
    }
    if (-not $quizCache[$quizNote.url].Contains($quizNote.text)) {
        throw ('Source mismatch: ' + $quizNote.author + ': ' + $quizNote.text)
    }
    if (-not $quizHeader.Contains($quizNote.text)) {
        throw ('Compiled catalog mismatch: ' + $quizNote.text)
    }
    Write-Output ('VERIFIED: ' + $quizNote.author + ' — ' + $quizNote.location)
}
Write-Output ('PASS: all ' + $quizNotes.Count + ' excerpts match the original source and compiled catalog.')
