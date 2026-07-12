#include <stdio.h>
#include <windows.h>
#include <wincrypt.h>

#ifdef _MSC_VER
#pragma comment(lib, "Crypt32.lib")
#endif

#define EXPORT __declspec(dllexport)

static BOOL LoadCertificateFile(const wchar_t* certificate_path, HCERTSTORE* certificate_store) {
	if (!certificate_store || !certificate_path) {
		return FALSE;
	}
	
	*certificate_store = CertOpenStore(
		CERT_STORE_PROV_FILENAME, 
		X509_ASN_ENCODING, 
		(HCRYPTPROV_LEGACY)NULL, 
		CERT_STORE_OPEN_EXISTING_FLAG | CERT_STORE_READONLY_FLAG, 
		certificate_path
	);
	
	return (*certificate_store != NULL);
}

static HCERTSTORE OpenSystemRootStore() {
	return CertOpenStore(
		CERT_STORE_PROV_SYSTEM, 
		X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		(HCRYPTPROV_LEGACY)NULL, 
		CERT_SYSTEM_STORE_LOCAL_MACHINE, 
		L"ROOT"
	);
}

EXPORT BOOL IsRootCACertInstalled(const wchar_t* certificate_path) {
	BOOL result = FALSE;
	HCERTSTORE certificate_store = NULL;
	HCERTSTORE system_root_store = NULL;
	PCCERT_CONTEXT input_certificate = NULL;
	PCCERT_CONTEXT installed_certificate = NULL;
	
	if (!LoadCertificateFile(certificate_path, &certificate_store)) {
		goto cleanup;
	}

	input_certificate = CertEnumCertificatesInStore(certificate_store, NULL);
	if (!input_certificate) {
		goto cleanup;
	}

	system_root_store = OpenSystemRootStore();
	if (!system_root_store) {
		goto cleanup;
	}

	installed_certificate = CertFindCertificateInStore(
		system_root_store,
		X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		0, 
		CERT_FIND_EXISTING, 
		input_certificate,
		NULL
	);
	
	result = (installed_certificate != NULL);
	
cleanup:
	if (installed_certificate) {
		CertFreeCertificateContext(installed_certificate);
	}
	if (input_certificate) {
		CertFreeCertificateContext(input_certificate);
	}
	if (certificate_store) {
		CertCloseStore(certificate_store, 0);
	}
	if (system_root_store) {
		CertCloseStore(system_root_store, 0);
	}
	
	return result;
}

EXPORT BOOL InstallRootCACert(const wchar_t* certificate_path) {
	BOOL result = FALSE;
	HCERTSTORE certificate_store = NULL;
	HCERTSTORE system_root_store = NULL;
	PCCERT_CONTEXT input_certificate = NULL;
	
	if (!LoadCertificateFile(certificate_path, &certificate_store)) {
		goto cleanup;
	}

	input_certificate = CertEnumCertificatesInStore(certificate_store, NULL);
	if (!input_certificate) {
		goto cleanup;
	}

	system_root_store = OpenSystemRootStore();
	if (!system_root_store) {
		goto cleanup;
	}
	
	result = CertAddEncodedCertificateToStore(
		system_root_store, 
		X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 
		input_certificate->pbCertEncoded, 
		input_certificate->cbCertEncoded, 
		CERT_STORE_ADD_REPLACE_EXISTING, 
		NULL
	);
	
cleanup:
	if (input_certificate) {
		CertFreeCertificateContext(input_certificate);
	}
	if (certificate_store) {
		CertCloseStore(certificate_store, 0);
	}
	if (system_root_store) {
		CertCloseStore(system_root_store, 0);
	}
	
	return result;
}

EXPORT BOOL RemoveRootCACert(const wchar_t* certificate_path) {
	BOOL result = FALSE;
	HCERTSTORE certificate_store = NULL;
	HCERTSTORE system_root_store = NULL;
	PCCERT_CONTEXT input_certificate = NULL;
	PCCERT_CONTEXT installed_certificate = NULL;
	
	if (!LoadCertificateFile(certificate_path, &certificate_store)) {
		goto cleanup;
	}

	input_certificate = CertEnumCertificatesInStore(certificate_store, NULL);
	if (!input_certificate) {
		goto cleanup;
	}

	system_root_store = OpenSystemRootStore();
	if (!system_root_store) {
		goto cleanup;
	}

	installed_certificate = CertFindCertificateInStore(
		system_root_store, 
		X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 
		0, 
		CERT_FIND_EXISTING, 
		input_certificate, 
		NULL
	);

	if (!installed_certificate) {
		goto cleanup;
	}

	result = CertDeleteCertificateFromStore(installed_certificate);
	// CertDeleteCertificateFromStore frees the context when deletion succeeds.
	installed_certificate = NULL;
	
cleanup:
	if (installed_certificate) {
		CertFreeCertificateContext(installed_certificate);
	}
	if (input_certificate) {
		CertFreeCertificateContext(input_certificate);
	}
	if (certificate_store) {
		CertCloseStore(certificate_store, 0);
	}
	if (system_root_store) {
		CertCloseStore(system_root_store, 0);
	}
	
	return result;
}

#ifdef TEST_BIN
int main() {
	printf("Testing certificate operations...\n");
	
	if (!InstallRootCACert(L"mitmproxy-ca-cert.cer")) {
		printf("ERROR: InstallRootCACert failed.\n"); 
		return -1;
	}
	printf("Certificate installed successfully.\n");
	
	if (!IsRootCACertInstalled(L"mitmproxy-ca-cert.cer")) {
		printf("ERROR: IsRootCACertInstalled failed.\n"); 
		return -1;
	}
	printf("Certificate verified in store.\n");
		
	if (!RemoveRootCACert(L"mitmproxy-ca-cert.cer")) {
		printf("ERROR: RemoveRootCACert failed.\n"); 
		return -1;        
	}
	printf("Certificate removed successfully.\n");
	
	printf("All tests passed!\n");
	return 0;
}
#endif
