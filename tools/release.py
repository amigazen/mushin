#!/usr/bin/env python3
"""Assemble the native Mushin package from GCC or SAS/C build outputs."""
import argparse
import re
import shutil
import struct
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
LIBRARIES = ('zunemaster.library', 'muimaster.library')


def library_version(path):
    data = path.read_bytes()
    if data[:4] != struct.pack('>I', 1011):
        raise ValueError(f'{path}: expected an Amiga Hunk load file')
    pattern = rb'\$VER: ' + re.escape(path.name.encode()) + rb' (\d+)\.(\d+)\b'
    versions = set(re.findall(pattern, data))
    if len(versions) != 1:
        raise ValueError(f'{path}: missing or ambiguous matching $VER string')
    return '.'.join(part.decode() for part in versions.pop())


def release(build_dir, output_dir, lha):
    build_dir, output_dir = build_dir.resolve(), output_dir.resolve()
    versions = [library_version(build_dir / name) for name in LIBRARIES]
    if versions[0] != versions[1]:
        raise ValueError('library versions differ; rebuild both variants')
    if not (build_dir / 'opentest').is_file():
        raise ValueError(f'{build_dir}: missing opentest')
    archiver = shutil.which(lha)
    if not archiver:
        raise ValueError(f'{lha}: LHA archiver not found; set LHA to its path')
    output_dir.mkdir(parents=True, exist_ok=True)
    archive = output_dir / f'Mushin-{versions[0]}-amigaos3-m68k.lha'
    # Always assemble a fresh tree: removed files must not survive a release.
    # Publish only after archiving and CRC checking complete successfully.
    with tempfile.TemporaryDirectory(prefix='.mushin-', dir=output_dir) as temp:
        stage = Path(temp)
        package = stage / 'Mushin'
        (package / 'Libs').mkdir(parents=True)
        (package / 'Tests').mkdir()
        for name in LIBRARIES:
            shutil.copyfile(build_dir / name, package / 'Libs' / name)
            (package / 'Libs' / name).chmod(0o755)
        shutil.copyfile(build_dir / 'opentest', package / 'Tests/opentest')
        (package / 'Tests/opentest').chmod(0o755)
        shutil.copyfile(ROOT / 'LICENSE.md', package / 'LICENSE')
        for name in ('ReadMe', 'ReadMe.info', 'Install', 'Install.info'):
            shutil.copyfile(ROOT / 'dist' / name, package / name)
        shutil.copyfile(ROOT / 'dist/Mushin.info', stage / 'Mushin.info')
        temporary_archive = stage / archive.name
        subprocess.run([archiver, 'ao5', str(temporary_archive),
                        'Mushin', 'Mushin.info'], cwd=stage, check=True)
        subprocess.run([archiver, 't', str(temporary_archive)], check=True)
        temporary_archive.replace(archive)
    print(f'Release: {archive}')
    return archive


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir', type=Path,
                        default=ROOT / 'build/muimaster')
    parser.add_argument('--output-dir', type=Path,
                        default=ROOT / 'build/muimaster/release')
    parser.add_argument('--lha', default='lha')
    args = parser.parse_args()
    try:
        release(args.build_dir, args.output_dir, args.lha)
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        parser.exit(1, f'release: {error}\n')


if __name__ == '__main__':
    main()
