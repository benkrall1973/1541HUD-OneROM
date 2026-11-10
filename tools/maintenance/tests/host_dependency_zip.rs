use onerom_fw::net::fetch_rom_file;
use std::path::PathBuf;

fn fixture(name: &str) -> Vec<u8> {
    let root = PathBuf::from(std::env::var("HUD_ZIP_FIXTURES").expect("fixture directory"));
    std::fs::read(root.join(name)).expect("read ZIP fixture")
}

#[test]
fn stored_and_deflated_members_preserve_payload_and_cache() {
    let expected: Vec<u8> = (0..8192).map(|i| (i % 251) as u8).collect();
    for name in ["stored.zip", "deflated.zip"] {
        let archive = fixture(name);
        let (bytes, cache) = fetch_rom_file("unused.zip", &archive,
            Some("roms/synthetic.bin".into()), true).expect("extract member");
        assert_eq!(bytes, expected);
        assert_eq!(cache, archive);
        let (bytes, cache) = fetch_rom_file("unused.zip", &archive,
            Some("roms/synthetic.bin".into()), false).expect("extract without cache");
        assert_eq!(bytes, expected);
        assert!(cache.is_empty());
    }
}

#[test]
fn missing_member_and_malformed_archives_are_errors() {
    assert!(fetch_rom_file("unused.zip", &fixture("stored.zip"),
        Some("missing.bin".into()), false).is_err());
    for name in ["truncated.zip", "bad-crc.zip", "not-a-zip.bin"] {
        assert!(fetch_rom_file("unused.zip", &fixture(name),
            Some("roms/synthetic.bin".into()), false).is_err(), "{name}");
    }
}
