# ════════════════════════════════════════════════════════════════════════════
# update_deps.ps1 — fetch / refresh.
# ════════════════════════════════════════════════════════════════════════════
param(
    [switch] $NoPull # Skip the `git pull` step when the checkout already exists.
)

$path_root           = split-path -Path $PSScriptRoot -Parent
$path_course_content = join-path $path_root 'course_content'
$misc                = join-path $PSScriptRoot 'helpers/misc.ps1'; . $misc

# Halt on any error (instead of PowerShell's default `Continue`).
$ErrorActionPreference = 'Stop'


# --- Dependency Definition ---
# One external dep for now: Casey Muratori's computer_enhance course repo.
$url_computer_enhance = 'https://github.com/cmuratori/computer_enhance.git'

# --- Run ---
if ($NoPull) { clone-gitrepo $path_course_content $url_computer_enhance -NoPull }
else         { clone-gitrepo $path_course_content $url_computer_enhance }

write-host ''
write-host 'Course content up to date.'
