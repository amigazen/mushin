#!/usr/bin/env python3
"""Exercise the real script with an optional host CLI build of InstallerLG.

The host has no Amiga assigns or resident libraries. Only LIBS:, resident
version lookup, and initial UI mode are substituted in temporary copies.
No files outside the fixture directories are installed.
"""
import argparse
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
NAMES = ('zunemaster.library', 'muimaster.library')


def version_file(name, version):
    return f'\0$VER: {name} {version} (03.10.2026)\0'.encode()


def scenario(installer, label, *, installed=None, answers='N\nN\n',
             backup=None, pretend=False, resident=0, novice=False,
             missing=False, invalid=False, directory=False):
    with tempfile.TemporaryDirectory(prefix='mushin-installer-') as temp:
        root = Path(temp)
        payload, target = root / 'Libs', root / 'system-libs'
        payload.mkdir()
        target.mkdir()
        for name in NAMES:
            (payload / name).write_bytes(version_file(name, '35.6'))
        destination = target / NAMES[0]
        backup_path = target / (NAMES[0] + '.mushin-old')
        if installed is not None:
            destination.write_bytes(installed)
        if backup is not None:
            backup_path.write_bytes(backup)
        if missing:
            (payload / NAMES[1]).unlink()
        if invalid:
            (payload / NAMES[1]).write_bytes(b'no version here')
        if directory:
            destination.mkdir()

        script = (ROOT / 'dist/Install').read_text()
        script = script.replace('"LIBS:"', f'"{target}"')
        script = script.replace('(getversion #name (resident))', str(resident))
        welcome = '(welcome "Install Mushin\'s native AmigaOS libraries.")'
        script = script.replace(welcome, welcome +
                                f'\n(set @pretend {int(pretend)})' +
                                f'\n(set @user-level {0 if novice else 2})')
        (root / 'Install').write_text(script)
        # InstallerLG's host argument reader expects a relative script path.
        result = subprocess.run([str(installer), 'Install'], cwd=root,
                                input=answers, text=True, capture_output=True,
                                timeout=10, check=True)
        out = result.stdout + result.stderr
        assert all(len(p.name) <= 30 for p in target.iterdir()), 'OFS/FFS filename limit'
        assert 'syntax error' not in out.lower(), out
        assert 'undefined variable' not in out.lower(), out
        if missing or invalid or directory:
            assert 'Aborting' in out, out
            assert not (target / NAMES[1]).exists()
            if not directory:
                assert not destination.exists()
        else:
            assert 'Bundled is v35.6.' in out, out
            if installed is not None:
                if b'$VER:' in installed:
                    old = installed.split(b' ')[2].decode()
                    assert f'v{old} already installed.' in out, out
                else:
                    assert 'version unknown' in out, out
            first, second = answers.splitlines()
            for name, answer in zip(NAMES, (first, second)):
                path = target / name
                original = installed if name == NAMES[0] else None
                expected = version_file(name, '35.6') if answer == 'Y' and not pretend else original
                assert path.exists() == (expected is not None), out
                if expected is not None:
                    assert path.read_bytes() == expected, out
            expected_backup = backup
            if expected_backup is None and installed is not None and first == 'Y' and not pretend:
                expected_backup = installed
            assert backup_path.exists() == (expected_backup is not None), out
            if expected_backup is not None:
                assert backup_path.read_bytes() == expected_backup, out
            if pretend:
                assert 'Pretend mode finished' in out, out
            if resident:
                assert 'Currently in memory: v40.1' in out, out
            if label == 'downgrade':
                assert 'This is a downgrade.' in out, out
            if label == 'same version':
                assert 'This replaces the same version.' in out, out
        print(f'Installer: {label} passed')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--installer', required=True, type=Path,
                        help='host InstallerLG executable built with UI=CLI')
    args = parser.parse_args()
    installer = args.installer.resolve()
    old = version_file(NAMES[0], '35.4')
    scenario(installer, 'fresh install', answers='Y\nY\n')
    scenario(installer, 'skip both', installed=old)
    scenario(installer, 'upgrade', installed=old, answers='Y\nN\n')
    scenario(installer, 'downgrade', installed=version_file(NAMES[0], '36.0'), answers='Y\nN\n')
    scenario(installer, 'same version', installed=version_file(NAMES[0], '35.6'), answers='Y\nN\n')
    scenario(installer, 'unknown version', installed=b'original unversioned data', answers='Y\nN\n')
    scenario(installer, 'retain backup', installed=old, backup=b'original backup', answers='Y\nN\n')
    scenario(installer, 'resident version', resident=40 * 65536 + 1)
    scenario(installer, 'pretend', installed=old, answers='Y\nY\n', pretend=True)
    scenario(installer, 'novice still asks', novice=True, answers='Y\nN\n')
    scenario(installer, 'missing payload', missing=True)
    scenario(installer, 'unversioned payload', invalid=True)
    scenario(installer, 'destination is directory', directory=True)


if __name__ == '__main__':
    main()
